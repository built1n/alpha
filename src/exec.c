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
static void reg_to_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=ctx->regs[(operand&0xF0)>>4];
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
  if(ctx->regs[(operand&0xF0)>>4]==ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void jump_less(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]<ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void jump_greater(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]>ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void jump_noequ(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]!=ctx->regs[0])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF]-2; // subtract the size of this instruction
    }
}
static void djnz(alpha_ctx* ctx, byte operand)
{
  --ctx->regs[(operand&0xF0)>>4];
  if(ctx->regs[(operand&0xF0)>>4])
    {
      ctx->regs[PC]=ctx->regs[operand&0xF];
    }
}
static void add_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]+=ctx->regs[(operand&0xF0)>>4];
}
static void sub_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]-=ctx->regs[(operand&0xF0)>>4];
}
static void mul_reg(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]*=ctx->regs[(operand&0xF0)>>4];
}
static void div_reg(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]==0) // divide by zero!
    {
      divideByZero(ctx);
      return;
    }
  ctx->regs[operand&0xF]/=ctx->regs[(operand&0xF0)>>4];
}
static void mod_reg(alpha_ctx* ctx, byte operand)
{
  if(ctx->regs[(operand&0xF0)>>4]==0) // divide by zero!
    {
      divideByZero(ctx);
      return;
    }
  ctx->regs[operand&0xF]%=ctx->regs[(operand&0xF0)>>4];
}
static void add_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]+=getArg(ctx);
}
static void sub_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]-=getArg(ctx);
}
static void mul_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]*=getArg(ctx);
}
static void div_imm(alpha_ctx* ctx, byte operand)
{
  word arg=getArg(ctx);
  if(!arg)
    {
      divideByZero(ctx);
      return;
    }
  ctx->regs[operand&0xF]/=getArg(ctx);
}
static void mod_imm(alpha_ctx* ctx, byte operand)
{
  word arg=getArg(ctx);
  if(!arg)
    {
      divideByZero(ctx);
      return;
    }
  ctx->regs[operand&0xF]%=getArg(ctx);
}
static void incr(alpha_ctx* ctx, byte operand)
{
  ++(ctx->regs[operand&0xF]);
}
static void decr(alpha_ctx* ctx, byte operand)
{
  --(ctx->regs[operand&0xF]);
}
static void push_reg(alpha_ctx* ctx, byte operand)
{
  pushStack(ctx,ctx->regs[operand&0xF]);
}
static void push_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[PC]-=1; // BUG?
  pushStack(ctx, getArg(ctx));
}
static void pop(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=popStack(ctx);
}
static void call_reg(alpha_ctx* ctx, byte operand)
{
  pushStack(ctx, ctx->regs[PC]+2);
  ctx->regs[PC]=ctx->regs[operand&0xF]-2;
}
static void call_imm(alpha_ctx* ctx, byte operand)
{
  pushStack(ctx, ctx->regs[PC]);
  --ctx->regs[PC];
  ctx->regs[PC]=getArg(ctx)-3;
}
static void ret(alpha_ctx* ctx, byte operand)
{
  ctx->regs[PC]=popStack(ctx)-1; // subtract the size of this instruction
}
static void alpha_putchar_imm(alpha_ctx* ctx, byte c)
{
  putchar(c);
}
static void alpha_putchar_reg(alpha_ctx* ctx, byte operand)
{
  putchar((ctx->regs[operand&0xF])&0xFF);
}
static void puts_reg(alpha_ctx* ctx, byte operand)
{
  for(;readByte(ctx, ctx->regs[operand&0xF]);++ctx->regs[operand&0xF])
    {
      putchar(readByte(ctx, ctx->regs[operand&0xF]));
    }
}
static void print_number(alpha_ctx* ctx, byte operand)
{
  byte reg_num=(operand&0xF);
  switch((operand&0xF0)>>4)
    {
    case 0: // decimal
      printf("%llu", ctx->regs[reg_num]);
      break;
    case 1:
      printf("%llX", ctx->regs[reg_num]);
      break;
    case 2:
      printf("%016llX", ctx->regs[reg_num]);
      break;
    case 3:
      printf("%llx", ctx->regs[reg_num]);
      break;
    case 4:
      printf("%016llx", ctx->regs[reg_num]);
      break;
    case 5:
      printf("%lld", (int64_t)ctx->regs[reg_num]);
      break;
    default:
      badInstr(ctx);
      return;
    }
}
static void halt_execution(alpha_ctx* ctx, byte operand)
{
  --ctx->regs[PC];
  ctx->done=true;
  ctx->return_value=ctx->regs[operand&0xF];
}
static void or(alpha_ctx* ctx, byte operand)
{
  ctx->regs[(operand&0xF0)>>4]|=ctx->regs[operand&0xF];
}
static void and(alpha_ctx* ctx, byte operand)
{
  ctx->regs[(operand&0xF0)>>4]&=ctx->regs[operand&0xF];
}
static void lsh(alpha_ctx* ctx, byte operand)
{
  ctx->regs[(operand&0xF0)>>4]<<=ctx->regs[operand&0xF];
}
static void rsh(alpha_ctx* ctx, byte operand)
{
  ctx->regs[(operand&0xF0)>>4]>>=ctx->regs[operand&0xF];
}
static void xor(alpha_ctx* ctx, byte operand)
{
  ctx->regs[(operand&0xF0)>>4]^=ctx->regs[operand&0xF];
}
static void not(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=~(ctx->regs[operand&0xF]);
}
static void rotl(alpha_ctx* ctx, byte operand)
{
  word bit=(ctx->regs[operand&0xF]&0x8000000000000000)>>63;
  ctx->regs[operand&0xF]<<=1;
  ctx->regs[operand&0xF]|=bit;
}
static void rotr(alpha_ctx* ctx, byte operand)
{
  word bit=(ctx->regs[operand&0xF]&0x1)<<63;
  ctx->regs[operand&0xF]>>=1;
  ctx->regs[operand&0xF]|=bit;
}
static void zero(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=0;
}
static void memsize(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=ctx->memsize;
}
static void nop(alpha_ctx* ctx, byte operand)
{
  --ctx->regs[PC]; // this is a one-byte instruction
}
static void reqcheck(alpha_ctx* ctx, byte operand)
{
  ctx->regs[operand&0xF]=ALPHA_IMPLEMENTED_REVISION;
}

void exec_opcode(alpha_ctx* ctx, byte opcode, byte operand)
{
  static void (*exec_table[256])(alpha_ctx*, byte)={
    &reg_to_reg, // 0x00
    &mem_to_reg, // 0x01
    &reg_to_mem, // 0x02
    &load_imm, // 0x03

    &jump_equ, // 0x04
    &jump_less, // 0x05
    &jump_greater, // 0x06
    &jump_noequ, // 0x07
    &djnz, // 0x08

    &add_reg, // 0x09
    &sub_reg, // 0x0A
    &mul_reg, // 0x0B
    &div_reg, // 0x0C
    &mod_reg, // 0x0D

    &add_imm, // 0x0E
    &sub_imm, // 0x0F
    &mul_imm, // 0x10
    &div_imm, // 0x11
    &mod_imm, // 0x12
    &incr, // 0x13
    &decr, // 0x14

    &push_reg, // 0x15
    &push_imm, // 0x16
    &pop, // 0x17
    &call_reg, // 0x18
    &call_imm, // 0x19
    &ret, // 0x1A

    &alpha_putchar_imm, // 0x1B
    &alpha_putchar_reg, // 0x1C
    &puts_reg, // 0x1D
    &print_number, // 0x1E
    
    &halt_execution, // 0x1F
    &or, // 0x20
    &and, // 0x21
    &lsh, // 0x22
    &rsh, // 0x23
    &xor, // 0x24
    &not, // 0x25
    &rotl, // 0x26
    &rotr, // 0x27
    &memsize, // 0x28
    &zero, // 0x29
    &nop, // 0x2A
    &reqcheck, // 0x2B
  };
  if(exec_table[opcode])
    exec_table[opcode](ctx, operand);
}
