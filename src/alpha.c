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
  for(int i=1;i<SP;++i)
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
  printf("R0: 0x%08X\n", ctx->regs[0]);
  printf("R1: 0x%08X\n", ctx->regs[1]);
  printf("R2: 0x%08X\n", ctx->regs[2]);
  printf("R3: 0x%08X\n", ctx->regs[3]);
  for(int i=0;i<16;++i)
    {
      printf("R%d: 0x%08X\n", i, ctx->regs[i]);
    }
  printf("Disassembly of instruction: ");
  word pc=ctx->regs[3];
  alpha_disasm(ctx);
  ctx->regs[3]=pc;
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
void alpha_disasm(alpha_ctx* ctx)
{
  if(!ctx->done && ctx->regs[3]<ctx->memsize)
    {
      disasm_opcode(ctx, readByte(ctx, ctx->regs[3]));
      ++(ctx->regs[3]);
    }
  else
    ctx->done=true;
}
