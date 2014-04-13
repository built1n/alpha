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

#include <alpha.h>
#include <mem.h>
#include <util.h>
static void load_store(alpha_ctx* ctx, byte opcode)
{
  byte operand=readByte(ctx, ctx->regs[PC]+1);
  byte dest=((opcode&3)<<3)&((operand&E0)<<5), src=(operand&0x1F);
  switch((opcode&0x1C)>>2)
    {
    case 0:
      ctx->regs[dest]=ctx->regs[src];
      break;
    case 1:
      ctx->regs[dest]=readWord(ctx, ctx->regs[src]);
      break;
    case 2:
      writeWord(ctx, ctx->regs[dest], ctx->regs[src]);
    default:
      badInstr(ctx);
      break;
    }
}
static void load_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[opcode&0x1F]=getArg(ctx);
}
static void jump(alpha_ctx* ctx, byte opcode)
{
  byte condition=opcode&0x1F;
}
void exec_opcode(alpha_ctx* ctx, byte opcode)
{
  static void (*exec_table[8])(alpha_ctx*, byte)={
    &load_store,
    &load_imm,
    &jump,
    &arith_bitwise,
    &inc,
    &dec,
    &bitwise,
    &io
  };
  exec_table[(opcode&0xE0)>>5](ctx, opcode);
}
