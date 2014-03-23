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
  ctx->regs[0]=rand();
  ctx->regs[1]=0;
  ctx->regs[2]=sp;
  ctx->regs[3]=0;
  ctx->done=false;
  ctx->return_value=EXIT_SUCCESS;
  return ctx;
}
void alpha_exec(alpha_ctx* ctx)
{
  if(!ctx->done && ctx->regs[3]<ctx->memsize)
    {
      exec_opcode(ctx, readByte(ctx, ctx->regs[3]));
      ++(ctx->regs[3]);
    }
  else
    ctx->done=true;
}
