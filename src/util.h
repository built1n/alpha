#include <alpha.h>
void exec_opcode(alpha_ctx*, byte, byte);
void disasm_opcode(alpha_ctx*, byte);
void badRead(alpha_ctx*);
void badWrite(alpha_ctx*);
void badInstr(alpha_ctx*);
void divideByZero(alpha_ctx*);
byte readByte(alpha_ctx*, word);
#define PC 15
#define SP 14
