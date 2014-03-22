#include <alpha.h>
word getArg(alpha_ctx* ctx)
{
  if((ctx->regs).regs[3]<(ctx->memsize)-4)
    {
      register word ret;
      register word pc = ctx->regs[3];
      ret=(ctx->memory[pc+1]>>24);
      ret|=(ctx->memory[pc+2]>>16);
      ret|=(ctx->memory[pc+3]>>8);
      ret|=(ctx->memory[pc+4]);
      return ret;
    }
  else
    badRead(ctx);
}
static inline void writeWord(alpha_ctx* ctx, word addr, word value)
{
  if(addr<ctx->memsize-4)
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
  if(addr<ctx->memsize-4)
    {
      word ret=ctx->memory[addr+3];
      ret|=((ctx->memory[addr+2])<<8);
      ret|=((ctx->memory[addr+1])<<16);
      ret|=((ctx->memory[addr])<<24);
      return ret;
    }
  else
    badRead(ctx);
}
void exec_0(alpha_ctx* ctx, byte opcode)
{
  ctx->regs[opcode&0x3]=ctx->regs[(opcode&0xC)>>2];
}
void exec_1(alpha_ctx* ctx, byte opcode) // reg to mem
{
  writeWord(ctx, ctx->regs[opcode&0x3], ctx->regs[(opcode&0xC)>>2]);
}
void exec_2(alpha_ctx* ctx, byte opcode)
{
  ctx->regs[opcode&0x3]=readWord(ctx, ctx->regs[(opcode&0x0C)>>2]);
}
void exec_3(alpha_ctx* ctx, byte opcode)
{
  ctx->regs[opcode&0x3]=getArg(ctx);
  ctx->regs[3]+=4;
}
void exec_4(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]==ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
void exec_5(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]!=ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
void exec_6(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]<ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
void exec_7(alpha_ctx* ctx, byte opcode)
{
  if(ctx->regs[0]>ctx->regs[(opcode&0x0C)>>2])
    {
      ctx->regs[3]=ctx->regs[opcode&0x03]-1; // no bounds checking here, done elsewhere
    }
}
void exec_opcode(alpha_ctx* ctx, byte opcode)
{
  void (*first_stage_exec[16])(alpha_ctx*, byte)={ // table of handlers for first nibble in opcode 
  &exec_0, &exec_1, &exec_2, &exec_3, &exec_4,
  &exec_5, &exec_6, &exec_7, &exec_8, &exec_9,
  &exec_A, &exec_B, &exec_C, &exec_D, &exec_E,
  &exec_F};
  first_stage_exec[(opcode&0xF0)>>4](ctx, opcode);
}
