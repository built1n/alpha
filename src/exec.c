#include <alpha.h>
#include <util.h>
#include <stdio.h>
static void nop(alpha_ctx* ctx, byte operand)
{
}
static inline word getArg(alpha_ctx* ctx)
{
  if(ctx->regs[PC]+1<(ctx->memsize)-4)
    {
      register word ret;
      register word pc = ctx->regs[PC]+1;
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
static void reg_to_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=ctx->regs[(opcode&0xF0)>>4];
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
  
void exec_opcode(alpha_ctx* ctx, byte opcode, byte operand)
{
  static void (*exec_table[256])(alpha_ctx*, byte)={
    &reg_to_reg, // 0x00
    &mem_to_reg, // 0x01
    &reg_to_mem, // 0x02
    &load_imm, // 0x03
    &jump_equ, // 0x04
  exec_table[opcode](ctx, operand);
}
