#include <alpha.h>
#include <util.h>
#include <stdlib.h>
#include <stdio.h>
void badRead(alpha_ctx* ctx)
{
  printf("Bad read.\n");
  ctx->done=true;
  ctx->return_value=ALPHA_OUT_OF_BOUNDS;
}
void badWrite(alpha_ctx* ctx)
{
  printf("Bad write.\n");
  ctx->done=true;
  ctx->return_value=ALPHA_OUT_OF_BOUNDS;
}
void badInstr(alpha_ctx* ctx)
{
  printf("Invalid instruction.\n");
  ctx->done=true;
  ctx->return_value=ALPHA_BAD_INSTR;
}
void divideByZero(alpha_ctx* ctx)
{
  printf("Divide by zero.\n");
  ctx->done=true;
  ctx->return_value=ALPHA_DIVIDE_BY_ZERO;
}
byte readByte(alpha_ctx* ctx, word addr)
{
  if(addr<ctx->memsize)
    return ctx->memory[addr];
  else
    badRead(ctx);
  return 0;
}
alpha_ctx* alpha_init(byte* mem, word sz, word stackSz, word sp)
{
  alpha_ctx* ctx=malloc(sizeof(alpha_ctx));
  ctx->memory=mem;
  ctx->memsize=sz;
  ctx->maxstacksize=stackSz;
  for(int i=0;i<SP;++i)
    {
      ctx->regs[i]=0;
    }
  ctx->regs[SP]=sp;
  ctx->regs[PC]=0;
  ctx->done=false;
  ctx->return_value=EXIT_SUCCESS;
  return ctx;
}
void alpha_print_state(alpha_ctx* ctx)
{
  printf("====Status====\n");
  for(int i=0;i<16;++i)
    {
      printf("R%d: 0x%08X\n", i, ctx->regs[i]);
    }
}
void alpha_exec(alpha_ctx* ctx)
{
  if(!ctx->done && ctx->regs[PC]+1<ctx->memsize)
    {
      exec_opcode(ctx, readByte(ctx, ctx->regs[PC]), readByte(ctx, ctx->regs[PC]+1));
      ctx->regs[PC]+=2;
    }
  else
    ctx->done=true;
}
void alpha_disasm(alpha_ctx* ctx)
{
  if(!ctx->done && ctx->regs[PC]+1<ctx->memsize)
    {
      disasm_opcode(ctx, readByte(ctx, ctx->regs[PC]), readByte(ctx, ctx->regs[PC]+1));
      ctx->regs[PC]+=2;
    }
  else
    ctx->done=true;
}
