#include <alpha.h>
#include <util.h>
#include <stdio.h>
static inline word getArg(alpha_ctx* ctx)
{
  if(ctx->regs[3]<(ctx->memsize)-4)
    {
      register word ret;
      register word pc = ctx->regs[PC];
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
      register word ret=readWord(ctx, ctx->regs[SP]);
      ctx->regs[SP]-=4;
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
      writeWord(ctx, ctx->regs[SP], value);
      ctx->regs[SP]+=4;
      ctx->stacksize+=4;
    }
  else
    badWrite(ctx);
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
