#include <alpha.h>
#include <util.h>
#include <stdio.h>
static inline word getArg(alpha_ctx* ctx)
{
  if(ctx->regs[PC]+2<(ctx->memsize)-4)
    {
      register word ret;
      register word pc = ctx->regs[PC]+2;
      ret=(ctx->memory[pc]<<24);
      ret|=(ctx->memory[pc+1]<<16);
      ret|=(ctx->memory[pc+2]<<8);
      ret|=(ctx->memory[pc+3]);
      ctx->regs[PC]+=4;
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
static void reg_to_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=ctx->regs[(operand&0xF0)>>4];
}
static void mem_to_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=readWord(ctx, ctx->regs[(operand&0xF0)]>>4);
}
static void reg_to_mem(alpha_ctx* ctx, byte operand)
{
  writeWord(ctx, ctx->regs[operand&0xF], ctx->regs[(operand&0xF0)>>4]);
}
static void load_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=getArg(ctx);
}
static void jump_equ(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]==ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void jump_less(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]<ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void jump_greater(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]>ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void jump_noequ(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]!=ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void djnz(alpha_ctx* ctx, byte operand)
{
  --ctx->regs[(operand&0xF0)>>4];
  if(ctx->regs[(operand&0xF0)>>4])
    {
      int32_t jump=ctx->regs[operand&0xF];
      ctx->regs[PC]+=jump;
      ctx->regs[PC]-=2; // size of this instruction
    }
}
static void add_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]+=ctx->regs[(operand&0xF0)>>4];
}
static void sub_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]-=ctx->regs[(operand&0xF0)>>4];
}
static void mul_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]*=ctx->regs[(operand&0xF0)>>4];
}
static void div_reg(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]==0) // divide by zero!
    {
      divideByZero(ctx);
      return;
    }
  ctx->regs[operand&0xF]/=ctx->regs[(operand&0xF0)>>4];
}
static void add_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]+=getArg(ctx);
}
static void sub_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]-=getArg(ctx);
}
static void mul_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]*=getArg(ctx);
}
static void div_imm(alpha_ctx* ctx, byte operand)
{
  word arg=getArg(ctx);
  if(!arg)
    {
      divideByZero(ctx);
      return;
    }
  ctx->regs[operand&0xF]/=getArg(ctx);
}
static void incr(alpha_ctx* ctx, byte operand)
{
  ++(ctx->regs[operand&0xF]);
}
static void decr(alpha_ctx* ctx, byte operand)
{
  --(ctx->regs[operand&0xF]);
}
static void push_reg(alpha_ctx* ctx, byte operand)
{
  pushStack(ctx,ctx->regs[operand&0xF]);
}
static void push_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[PC]-=1; // BUG?
  pushStack(ctx, getArg(ctx));
}
static void pop(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=popStack(ctx);
}
static void call_reg(alpha_ctx* ctx, byte operand)
{
  pushStack(ctx, ctx->regs[PC]);
  ctx->regs[PC]=ctx->regs[operand&0xF]-2;
}
static void call_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[PC]-=1;
  ctx->regs[PC]=getArg(ctx);
  ctx->regs[PC]-=4;
}
static void ret(alpha_ctx* ctx, byte operand)
{
  ctx->regs[PC]=popStack(ctx);
}
static void alpha_putchar_imm(alpha_ctx* ctx, byte c)
{
  putchar(c);
}
static void alpha_putchar_reg(alpha_ctx* ctx, byte operand)
{
  putchar((ctx->regs[operand&0xF])&0xFF);
}
static void puts_reg(alpha_ctx* ctx, byte operand)
{
  for(;readByte(ctx, ctx->regs[operand&0xF]);++ctx->regs[operand&0xF])
    {
      putchar(readByte(ctx, ctx->regs[operand&0xF]));
    }
}
static void print_number(alpha_ctx* ctx, byte operand)
{
  byte reg_num=(operand&0xF);
  switch((operand&0xF0)>>4)
    {
    case 0: // decimal
      printf("%u", ctx->regs[reg_num]);
      break;
    case 1:
      printf("%X", ctx->regs[reg_num]);
      break;
    case 2:
      printf("%08X", ctx->regs[reg_num]);
      break;
    case 3:
      printf("%x", ctx->regs[reg_num]);
      break;
    case 4:
      printf("%08x", ctx->regs[reg_num]);
      break;
    case 5:
      printf("%d", (int32_t)ctx->regs[reg_num]);
      break;
    default:
      badInstr(ctx);
      return;
    }
}
static void halt_execution(alpha_ctx* ctx, byte operand)
{
  --ctx->regs[PC];
  ctx->done=true;
  ctx->return_value=ctx->regs[operand&0xF];
}
void exec_opcode(alpha_ctx* ctx, byte opcode, byte operand)
{
  static void (*exec_table[256])(alpha_ctx*, byte)={
    &reg_to_reg, // 0x00
    &mem_to_reg, // 0x01
    &reg_to_mem, // 0x02
    &load_imm, // 0x03

    &jump_equ, // 0x04
    &jump_less, // 0x05
    &jump_greater, // 0x06
    &jump_noequ, // 0x07
    &djnz, // 0x08

    &add_reg, // 0x09
    &sub_reg, // 0x0A
    &mul_reg, // 0x0B
    &div_reg, // 0x0C

    &add_imm, // 0x0D
    &sub_imm, // 0x0E
    &mul_imm, // 0x0F
    &div_imm, // 0x10
    &incr, // 0x11
    &decr, // 0x12

    &push_reg, // 0x13
    &push_imm, // 0x14
    &pop, // 0x15
    &call_reg, // 0x16
    &call_imm, // 0x17
    &ret, // 0x18

    &alpha_putchar_imm, // 0x19
    &alpha_putchar_reg, // 0x1A
    &puts_reg, // 0x1B
    &print_number, // 0x1C
    
    &halt_execution // 0x1D
  };
  if(exec_table[opcode])
    exec_table[opcode](ctx, operand);
}
