#include <alpha.h>
#include <util.h>
#include <stdio.h>
static inline word getArg(alpha_ctx* ctx)
{
  if(ctx->regs[3]<(ctx->memsize)-4)
    {
      register word ret;
      register word pc = ctx->regs[3];
      ret=(ctx->memory[pc+1]<<24);
      ret|=(ctx->memory[pc+2]<<16);
      ret|=(ctx->memory[pc+3]<<8);
      ret|=(ctx->memory[pc+4]);
      return ret;
    }
  else
    badRead(ctx);
  return 0xDEADBEEF;
}
static inline void writeWord(alpha_ctx* ctx, word addr, word value)
{
  if(addr<ctx->memsize-3)
    {
      ctx->memory[addr]=((value&0xFF000000) >> 24);
      ctx->memory[addr+1]=((value&0x00FF0000) >> 16);
      ctx->memory[addr+2]=((value&0x0000FF00) >> 8);
      ctx->memory[addr+3]=((value&0xFF));
    }
  else
    badWrite(ctx);
}
static inline word readWord(alpha_ctx* ctx, word addr)
{
  if(addr<ctx->memsize-3)
    {
      word ret=ctx->memory[addr+3];
      ret|=((ctx->memory[addr+2])<<8);
      ret|=((ctx->memory[addr+1])<<16);
      ret|=((ctx->memory[addr])<<24);
      return ret;
    }
  else
    badRead(ctx);
  return 0xDEADBEEF;
}
static inline word popStack(alpha_ctx* ctx)
{
  if(ctx->stacksize>=4)
    {
      register word ret=readWord(ctx, ctx->regs[2]);
      ctx->regs[2]-=4;
      return ret;
    }
  else
    badRead(ctx);
  return 0xDEADBEEF;
}
static inline void pushStack(alpha_ctx* ctx, word value)
{
  if(ctx->stacksize<ctx->maxstacksize-4)
    {
      writeWord(ctx, ctx->regs[2], value);
      ctx->regs[2]+=4;
      ctx->stacksize+=4;
    }
  else
    badWrite(ctx);
}
static void exec_0(alpha_ctx* ctx, byte opcode)
{
  ctx->regs[opcode&0x3]=ctx->regs[(opcode&0xC)>>2];
}
static void exec_1(alpha_ctx* ctx, byte opcode) // reg to mem
{
  writeWord(ctx, ctx->regs[opcode&0x3], ctx->regs[(opcode&0xC)>>2]);
}
static void exec_2(alpha_ctx* ctx, byte opcode)
{
  ctx->regs[opcode&0x3]=readWord(ctx, ctx->regs[(opcode&0x0C)>>2]);
}
static void exec_3(alpha_ctx* ctx, byte opcode)
{
  ctx->regs[opcode&0x3]=getArg(ctx);
  ctx->regs[3]+=4;
}
static void exec_4(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]==ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
static void exec_5(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]!=ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
static void exec_6(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]<ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
static void exec_7(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]>ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
static void exec_8(alpha_ctx* ctx, byte opcode)
{
  byte detail=(opcode&0x0C)>>2;
  switch(detail)
    {
    case 0:
      pushStack(ctx, ctx->regs[opcode&0x3]);
      break;
    case 1:
      ctx->regs[opcode&0x03]=popStack(ctx);
      break;
    case 2:
      ctx->regs[0]+=ctx->regs[opcode&0x03];
      break;
    case 3:
      ctx->regs[0]-=ctx->regs[opcode&0x03];
      break;
      // no need for default with 2 bits!
    }
}
static void exec_9(alpha_ctx* ctx, byte opcode)
{
  switch((opcode&0x0C)>>2)
  {
  case 0:
    ctx->regs[0]*=ctx->regs[opcode&0x03];
    break;
  case 1:
    ctx->regs[0]/=ctx->regs[opcode&0x03];
    break;
  case 2:
    ctx->regs[0]%=ctx->regs[opcode&0x03];
      break;
  case 3:
    --(ctx->regs[opcode&0x03]);
    break;
  }
}
static void exec_A(alpha_ctx* ctx, byte opcode)
{
  ++(ctx->regs[opcode&0x03]);
}
static void exec_extd(alpha_ctx* ctx, byte opcode)
{
  switch((opcode&0x3C)>>2)
    {
    case 0x0:
      printf("%u", ctx->regs[0]);
      fflush(stdout);
      break;
    case 0x01:
      ctx->regs[0]=ctx->memsize;
      break;
    case 0x02: // call
      pushStack(ctx, (ctx->regs[3]));
      ctx->regs[3]=ctx->regs[opcode&0x03]-1;
      break;
    case 0x03: // ret
      ctx->regs[3]=popStack(ctx);
      break;
    case 0x04:
      ctx->return_value=ctx->regs[0];
      ctx->done=true;
      break;
    case 0x05:
      --(ctx->regs[0]);
      if(ctx->regs[0])
	ctx->regs[3]=ctx->regs[opcode&0x03]-1;
      break;
    case 0x06:
      {
	char c;
	for(word ptr=ctx->regs[opcode&0x03];c=readByte(ctx, ptr), c;++ptr)
	  {
	    printf("%c",c);
	  }
	break;
      }
    case 0x07:
      ctx->regs[3]=ctx->regs[opcode&0x03]-1;
      break;
    case 0x08:
      switch(opcode&0x03)
	{
	case 0:
	  putchar('\n');
	  break;
	case 1:
	  {
	    byte diff=readByte(ctx, ctx->regs[3]+1)-1;
	    if(diff&0x80)
	      ctx->regs[3]-=(diff&0x7F);
	    else
	      ctx->regs[3]+=(diff&0x7F);
	    --ctx->regs[3];
	    break;
	  }
	case 2:
	  ctx->regs[0]=~ctx->regs[0];
	  break;
	case 3:
          {
	    register byte firstBit=(ctx->regs[0]&1);
	    ctx->regs[0]>>=1;
	    ctx->regs[0]&=firstBit<<31;
	    break;
          }
	default:
	  badInstr(ctx);
	}
      break;
    case 0x09:
      ctx->regs[0]&=ctx->regs[opcode&3];
      break; 
    case 0x0A:
      ctx->regs[0]|=ctx->regs[opcode&3];
      break;
    case 0x0B:
      ctx->regs[0]^=ctx->regs[opcode&3];
      break;
    default:
      badInstr(ctx);
    }
}
static void exec_F(alpha_ctx* ctx, byte opcode)
{
  switch(opcode&0xC)
    {
    case 0:
      ctx->regs[0]<<=ctx->regs[opcode&3];
      break;
    case 1:
      ctx->regs[0]>>=ctx->regs[opcode&3];
      break;
    default:
      badInstr(ctx);
    }
}
static void nop(alpha_ctx* ctx, byte opcode)
{
  return;
}
void exec_opcode(alpha_ctx* ctx, byte opcode)
{
  static void (*exec_table[16])(alpha_ctx*, byte)={ // table of handlers for first nibble in opcode 
  &exec_0, &exec_1, &exec_2, &exec_3, &exec_4,
  &exec_5, &exec_6, &exec_7, &exec_8, &exec_9,
  &exec_A, &nop, &exec_extd, &exec_extd, &exec_extd/* 0xC, 0xD, and 0x0E are extd. instructions*/, &exec_F};
  exec_table[(opcode&0xF0)>>4](ctx, opcode);
}
