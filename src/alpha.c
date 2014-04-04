/*
 *  Alpha emulation library
 *  Copyright (C) 2014 Franklin Wei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

#include <alpha.h>
void badRead(alpha_ctx* ctx)
{
  printf("Bad read at 0x%08X.\n", ctx->regs[PC]);
  ctx->done=true;
  ctx->return_value=ALPHA_OUT_OF_BOUNDS;
}
void badWrite(alpha_ctx* ctx)
{
  printf("Bad write at 0x%08X.\n", ctx->regs[PC]);
  ctx->done=true;
  ctx->return_value=ALPHA_OUT_OF_BOUNDS;
}
void badInstr(alpha_ctx* ctx)
{
  printf("Invalid instruction at 0x%08X.\n", ctx->regs[PC]);
  ctx->done=true;
  ctx->return_value=ALPHA_BAD_INSTR;
}
void divideByZero(alpha_ctx* ctx)
{
  printf("Divide by zero at 0x%08X.\n", ctx->regs[PC]);
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
void stackOverflow(alpha_ctx* ctx)
{
  printf("Stack overflow at 0x%08X.\n", ctx->regs[PC]);
  ctx->done=true;
  ctx->return_value=ALPHA_STACK_OVERFLOW;
}
void stackUnderflow(alpha_ctx* ctx)
{
  printf("Stack underflow at 0x%08X.\n", ctx->regs[PC]);
  ctx->done=true;
  ctx->return_value=ALPHA_STACK_UNDERFLOW;
}
alpha_ctx* alpha_init(byte* mem, word sz, word sp)
{
  alpha_ctx* ctx=malloc(sizeof(alpha_ctx));
  ctx->memory=mem;
  ctx->memsize=sz;
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
  printf("Available memory: %u bytes\n", ctx->memsize);
  printf("Disassembly of instruction: ");
  word oldpc=ctx->regs[PC];
  alpha_disasm(ctx);
  ctx->regs[PC]=oldpc;
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
      printf("0x%08X: ", ctx->regs[PC]);
      disasm_opcode(ctx, readByte(ctx, ctx->regs[PC]), readByte(ctx, ctx->regs[PC]+1));
      ctx->regs[PC]+=2;
    }
  else
    ctx->done=true;
}
