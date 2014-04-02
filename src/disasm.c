#include <alpha.h>
#include <util.h>
#include <stdio.h>
#include <memory.h>
static void reg_to_reg(alpha_ctx* ctx, byte operand)
{
  printf("MOV R%d, R%d\n", operand&0xF, (operand&0xF0)>>4);
}
static void mem_to_reg(alpha_ctx* ctx, byte operand)
{
  printf("MOV R%d, (R%d)\n", operand&0xF, (operand&0xF0)>>4);
}
static void reg_to_mem(alpha_ctx* ctx, byte operand)
{
  printf("MOV (R%d), R%d\n", operand&0xF, (operand&0xF0)>>4);
}
static void load_imm(alpha_ctx* ctx, byte operand)
{
  printf("MOV R%d, $0x%08X\n", operand&0xF, getArg(ctx));
}
static void jump_equ(alpha_ctx* ctx, byte operand)
{
  printf("JE R%d, R%d\n", (operand&0xF0)>>4, operand&0xF);
}
static void jump_less(alpha_ctx* ctx, byte operand)
{
  printf("JL R%d, R%d\n", (operand&0xF0)>>4, operand&0xF);
}
static void jump_greater(alpha_ctx* ctx, byte operand)
{
  printf("JG R%d, R%d\n", (operand&0xF0)>>4, operand&0xF);
}
static void jump_noequ(alpha_ctx* ctx, byte operand)
{
  printf("JNE R%d, R%d\n", (operand&0xF0)>>4, operand&0xF);
}
static void djnz(alpha_ctx* ctx, byte operand)
{
  printf("DJNZ R%d, R%d\n", (operand&0xF0)>>4, operand&0xF);
}
static void add_reg(alpha_ctx* ctx, byte operand)
{
  printf("ADD R%d, R%d\n", operand&0xF, (operand&0xF0)>>4);
}
static void sub_reg(alpha_ctx* ctx, byte operand)
{
  printf("SUB R%d, R%d\n", operand&0xF, (operand&0xF0)>>4);
}
static void mul_reg(alpha_ctx* ctx, byte operand)
{
  printf("MUL R%d, R%d\n", operand&0xF, (operand&0xF0)>>4);
}
static void div_reg(alpha_ctx* ctx, byte operand)
{
  printf("DIV R%d, R%d\n", operand&0xF, (operand&0xF0)>>4);
}
static void add_imm(alpha_ctx* ctx, byte operand)
{
  printf("ADD R%d, $0x%08X\n", operand&0xF, getArg(ctx));
}
static void sub_imm(alpha_ctx* ctx, byte operand)
{
  printf("SUB R%d, $0x%08X\n", operand&0xF, getArg(ctx));
}
static void mul_imm(alpha_ctx* ctx, byte operand)
{
  printf("MUL R%d, $0x%08X\n", operand&0xF, getArg(ctx));
}
static void div_imm(alpha_ctx* ctx, byte operand)
{
  printf("DIV R%d, $0x%08X\n", operand&0xF, getArg(ctx));
}
static void incr(alpha_ctx* ctx, byte operand)
{
  printf("INCR R%d\n", operand&0xF);
}
static void decr(alpha_ctx* ctx, byte operand)
{
  printf("DECR R%d\n", operand&0xF);
}
static void push_reg(alpha_ctx* ctx, byte operand)
{
  printf("PUSH R%d\n", operand&0xF);
}
static void push_imm(alpha_ctx* ctx, byte operand)
{
  ctx->regs[PC]-=1; // BUG?
  printf("PUSH $ 0x%08X\n", getArg(ctx));
}
static void pop(alpha_ctx* ctx, byte operand)
{
  printf("POP R%d\n", operand&0xF);
}
static void call_reg(alpha_ctx* ctx, byte operand)
{
  printf("CALL (R%d)\n", operand&0xF);
}
static void call_imm(alpha_ctx* ctx, byte operand)
{
  --ctx->regs[PC];
  printf("CALL $0x%08X\n", getArg(ctx));
}
static void ret(alpha_ctx* ctx, byte operand)
{
  printf("RET\n");
}
static void alpha_putchar_imm(alpha_ctx* ctx, byte c)
{
  printf("PUTC $'%c'\n",c);
}
static void alpha_putchar_reg(alpha_ctx* ctx, byte operand)
{
  printf("PUTC R%d\n", operand&0xF);
}
static void puts_reg(alpha_ctx* ctx, byte operand)
{
  printf("PUTS R%d\n", operand&0xF);
}
static void print_number(alpha_ctx* ctx, byte operand)
{
  byte reg_num=(operand&0xF);
  switch((operand&0xF0)>>4)
    {
    case 0: // decimal
      printf("PUTDECU ");
      break;
    case 1:
      printf("PUTHEXU ");
      break;
    case 2:
      printf("PUTHEXUP ");
      break;
    case 3:
      printf("PUTHEXL ");
      break;
    case 4:
      printf("PUTHEXLP ");
      break;
    case 5:
      printf("PUTDECS ");
      break;
    default:
      printf("Unknown instruction\n");
      return;
    }
  printf("R%d\n", reg_num);
}
static void halt_execution(alpha_ctx* ctx, byte operand)
{
  printf("HLT R%d\n", ctx->regs[operand&0xF]);
}
void disasm_opcode(alpha_ctx* ctx, byte opcode, byte operand)
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

    &add_imm, // 0x0D
    &sub_imm, // 0x0E
    &mul_imm, // 0x0F
    &div_imm, // 0x10
    &incr, // 0x11
    &decr, // 0x12

    &push_reg, // 0x13
    &push_imm, // 0x14
    &pop, // 0x15
    &call_reg, // 0x16
    &call_imm, // 0x17
    &ret, // 0x18

    &alpha_putchar_imm, // 0x19
    &alpha_putchar_reg, // 0x1A
    &puts_reg, // 0x1B
    &print_number, // 0x1C
    
    &halt_execution // 0x1D
    // more!
  };
  if(exec_table[opcode])
    exec_table[opcode](ctx, operand);
}
