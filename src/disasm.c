#include <alpha.h>
#include <util.h>
#include <stdio.h>
static inline word getArg(alpha_ctx* ctx)
{
  if(ctx->regs[3]<(ctx->memsize)-4)
    {
      register word ret;
      register word pc = ctx->regs[3];
      ret=(ctx->memory[pc+1]<<24);
      ret|=(ctx->memory[pc+2]<<16);
      ret|=(ctx->memory[pc+3]<<8);
      ret|=(ctx->memory[pc+4]);
      return ret;
    }
  else
    badRead(ctx);
  return 0xDEADBEEF;
}
static void exec_0(alpha_ctx* ctx, byte opcode)
{
  if((opcode&3)!=(opcode&0xC)>>2)
    printf("MOV R%1d, R%1d\n", opcode&0x03, (opcode&0xC)>>2);
  else
    printf("NOP\n");
}
static void exec_1(alpha_ctx* ctx, byte opcode) // reg to mem
{
  printf("MOV (R%1d), R%1d\n", opcode&0x03, (opcode&0xC)>>2);
}
static void exec_2(alpha_ctx* ctx, byte opcode)
{
  printf("MOV R%1d, (R%1d\n)", opcode&0x03, (opcode&0xC)>>2);
}
static void exec_3(alpha_ctx* ctx, byte opcode)
{
  printf("MOV R%1d, $0x%08X\n", opcode&3, getArg(ctx));
  ctx->regs[3]+=4;
}
static void exec_4(alpha_ctx* ctx, byte opcode)
{
  printf("JE R%1d, R%1d\n", opcode&0xC, opcode&0x3);
}
static void exec_5(alpha_ctx* ctx, byte opcode)
{
  printf("JNE R%1d, R%1d\n", opcode&0xC, opcode&0x3);
}
static void exec_6(alpha_ctx* ctx, byte opcode)
{
  printf("JL R%1d, R%1d\n", opcode&0xC, opcode&0x3);
}
static void exec_7(alpha_ctx* ctx, byte opcode)
{
  printf("JG R%1d, R%1d\n", opcode&0xC, opcode&0x3);
}
static void exec_8(alpha_ctx* ctx, byte opcode)
{
  byte detail=(opcode&0x0C)>>2;
  switch(detail)
    {
    case 0:
      printf("PUSH R%1d\n", opcode&0x3);
      break;
    case 1:
      printf("POP R%1d\n", opcode&0x3);
      break;
    case 2:
      printf("ADD R%1d\n", opcode&0x3);
      break;
    case 3:
      printf("SUB R%1d\n", opcode&0x3);
      break;
      // no need for default with 2 bits!
    }
}
static void exec_9(alpha_ctx* ctx, byte opcode)
{
  switch((opcode&0x0C)>>2)
  {
  case 0:
    printf("MUL R%1d\n", opcode&0x3);
    break;
  case 1:
    printf("DIV R%1d\n", opcode&0x3);
    break;
  case 2:
    printf("MOD R%1d\n", opcode&0x3);
    break;
  case 3:
    printf("DEC R%1d\n", opcode&0x3);
    break;
  }
}
static void exec_A(alpha_ctx* ctx, byte opcode)
{
  printf("INC R%1d\n", opcode&0x3);
}
static void exec_extd(alpha_ctx* ctx, byte opcode)
{
  switch((opcode&0x3C)>>2)
    {
    case 0x0:
      printf("PRINT\n");
      break;
    case 0x01:
      printf("GETMEMSZ\n");
    case 0x02: // call
      printf("CALL R%1d\n", opcode&0x03);
      break;
    case 0x03: // ret
      printf("RET\n");
      break;
    case 0x04:
      printf("HLT\n");
      break;
    case 0x05:
      printf("DJNZ R%1d\n", opcode&0x03);
      break;
    case 0x06:
      {
	printf("PUTS R%1d\n", opcode&0x03);
	break;
      }
    case 0x07:
      printf("JMP R%1d\n", opcode&3);
      break;
    case 0x08:
      switch(opcode&3)
	{
	case 0:
	  printf("NL\n");
	  break;
	case 1:
	  printf("JR 0x%02X\n", readByte(ctx, ctx->regs[3]+1));
	  ++ctx->regs[3];
	  break;
	}
      break;
    default:
      badInstr(ctx);
    }
}
static void nop(alpha_ctx* ctx, byte opcode)
{
  return;
}
static void unknown_opcode(alpha_ctx* ctx, byte opcode)
{
  printf("DATA 0x%02X\n", opcode);
}
void disasm_opcode(alpha_ctx* ctx, byte opcode)
{
  static void (*exec_table[16])(alpha_ctx*, byte)={ // table of handlers for first nibble in opcode 
  &exec_0, &exec_1, &exec_2, &exec_3, &exec_4,
  &exec_5, &exec_6, &exec_7, &exec_8, &exec_9,
  &exec_A, &unknown_opcode, &exec_extd, &exec_extd, &exec_extd/* 0xC, 0xD, and 0x0E are extd. instructions*/, &unknown_opcode);
  exec_table[(opcode&0xF0)>>4](ctx, opcode);
}
