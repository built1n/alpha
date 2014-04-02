#ifndef ALPHA_MEMORY_H_
#define ALPHA_MEMORY_H_
#include <alpha.h>
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
      register word ret=readWord(ctx, ctx->regs[SP]-4);
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

#endif
