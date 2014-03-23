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

#ifndef ALPHA_H_
#define ALPHA_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t byte;
typedef uint32_t word;

typedef struct alpha_ctx {
  byte* memory;
  word memsize, maxstacksize;
  word stacksize;
  word regs[4];
  bool done;
  int return_value;
} alpha_ctx;

#ifdef __cplusplus
extern "C" {
#endif

  alpha_ctx* alpha_init(byte* memory, word memsize, word maxstacksize, word sp);
  void alpha_exec(alpha_ctx*);
  void alpha_print_state(alpha_ctx*);
  void alpha_disasm(alpha_ctx*);

#define ALPHA_OUT_OF_BOUNDS 0xFF
#define ALPHA_BAD_INSTR 0xFF00

#ifdef __cplusplus
}
#endif

#endif /* ALPHA_H_ */
