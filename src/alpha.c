#include <alpha.h>
void badRead(alpha_ctx* ctx)
{
}
void badWrite(alpha_ctx* ctx)
{
}
void badInstr(alpha_ctx* ctx)
{
}
inline static byte readByte(alpha_ctx* ctx, word addr)
{
  if(addr<ctx->memsize)
    return ctx->memory[addr];
  else
    badRead(ctx);
}
void alpha_exec(alpha_ctx* ctx)
{
  exec_opcode(ctx, readByte(ctx, ctx->regs[3]));
  ++(ctx->regs[3]);
}
