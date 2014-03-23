#include <alpha.h>
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
inline static byte readByte(alpha_ctx* ctx, word addr)
{
  if(addr<ctx->memsize)
    return ctx->memory[addr];
  else
    badRead(ctx);
}
alpha_ctx* alpha_init(byte* mem, word sz, word stackSz, word sp)
{
  alpha_ctx* ctx=malloc(sizeof(alpha_ctx));
  ctx->memory=mem;
  ctx->memsize=sz;
  ctx->maxstacksize=stackSz;
  ctx->regs[2]=sp;
  ctx->regs[0]=rand();
  ctx->regs[3]=0;
  ctx->return_value=EXIT_SUCCESS;
}
void alpha_exec(alpha_ctx* ctx)
{
  if(!ctx->done && ctx->regs[3]<ctx->memsize)
    {
      exec_opcode(ctx, readByte(ctx, ctx->regs[3]));
      ++(ctx->regs[3]);
    }
}
