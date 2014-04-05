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
#ifndef ALPHA_MEMORY_H_
#define ALPHA_MEMORY_H_
#include <stdio.h>

#include <alpha.h>
#include <util.h>
static inline void writeWord(alpha_ctx* ctx, word addr, word value)
{
  if(addr<ctx->memsize-(ALPHA_WORDSIZE-1))
    {
      word mask=((word)0xFF<<((ALPHA_WORDSIZE-1)*8));
      word sizeMinusOne=(ALPHA_WORDSIZE-1)*8;
      for(int i=0;i<ALPHA_WORDSIZE;++i)
	{
	  ctx->memory[addr+i]|=((value&mask) >> ( sizeMinusOne - i*8));
	  mask>>=8;
	}
    }
  else
    badWrite(ctx);
}
static inline word readWord(alpha_ctx* ctx, word addr)
{
  if(addr<ctx->memsize-(ALPHA_WORDSIZE-1))
    {
      register word ret;
      ret=((word)ctx->memory[addr]<<(ALPHA_WORDSIZE-1)*8);
      for(int i=1;i<ALPHA_WORDSIZE;++i)
	ret|=((word)ctx->memory[addr+i] << (((ALPHA_WORDSIZE-1)*8)-8*i));
      return ret;
    }
  else
    badRead(ctx);
  return 0xDEADBEEF;
}
static inline word getArg(alpha_ctx* ctx)
{
  if(ctx->regs[PC]+2<(ctx->memsize)-ALPHA_WORDSIZE)
    {
      register word ret=readWord(ctx, ctx->regs[PC]+2);
      ctx->regs[PC]+=ALPHA_WORDSIZE;
      return ret;
    }
  else
    badRead(ctx);
  return 0xDEADBEEF;
}

static inline word popStack(alpha_ctx* ctx)
{
  if(ctx->regs[SP]>=ALPHA_WORDSIZE)
    {
      register word ret=readWord(ctx, ctx->regs[SP]-ALPHA_WORDSIZE);
      ctx->regs[SP]-=ALPHA_WORDSIZE;
      return ret;
    }
  else
    stackUnderflow(ctx);
  return 0xDEADBEEF;
}
static inline void pushStack(alpha_ctx* ctx, word value)
{
  if(ctx->regs[SP]<ctx->memsize-(ALPHA_WORDSIZE-1))
    {
      writeWord(ctx, ctx->regs[SP], value);
      ctx->regs[SP]+=ALPHA_WORDSIZE;
    }
  else
    stackOverflow(ctx);
}

#endif
