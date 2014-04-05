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
#include <alpha.h>
void exec_opcode(alpha_ctx*, byte);
void disasm_opcode(alpha_ctx*, byte);
void badRead(alpha_ctx*);
void badWrite(alpha_ctx*);
void badInstr(alpha_ctx*);
void divideByZero(alpha_ctx*);
byte readByte(alpha_ctx*, word);
void stackOverflow(alpha_ctx*);
void stackUnderflow(alpha_ctx*);
#define PC 15
#define SP 14
