/*
  PokeMini - Pokémon-Mini Emulator
  Copyright (C) 2009-2012  JustBurn
  Snailmon - Pokémon Mini emulator for the GBA
  Copyright (C) 2026  LoxoSoftware

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Note: Any write to MinxCPU.HL.B.I needs to be reflected into MinxCPU.N.B.I

#include <gba.h>
#include "MinxCPU_noInline.h"

#pragma GCC push_options
#pragma GCC optimize ("O2")

IWRAM_CODE ARM_CODE
int MinxCPU_Exec(void)
{
	uint8_t I8A, I8B;
	uint16_t I16;

	// Shift U
	if (MinxCPU.Shift_U) {
		MinxCPU.U1 = MinxCPU.U2;
		MinxCPU.U2 = MinxCPU.PC.B.I;
		MinxCPU.Shift_U--;
		MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
	}

	// Check MinxCPU_HALT or MinxCPU_STOP status
	if (MinxCPU.Status != MINX_STATUS_NORMAL) {
		if (MinxCPU.Status == MINX_STATUS_IRQ) {
			MinxCPU.Status = MINX_STATUS_NORMAL;	// Return to normal
			MinxCPU_CALLI(MinxCPU.IRQ_Vector);		// Jump to IRQ vector
			return 20;
		} else {
			return 8;				// Cause short NOPs
		}
	}

	// Read IR
	MinxCPU.IR = Fetch8();

	// Process instruction
	switch(MinxCPU.IR) {

		case 0x00: // ADD A, A
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x01: // ADD A, B
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x02: // ADD A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x03: // ADD A, [HL]
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x04: // ADD A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x05: // ADD A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x06: // ADD A, [X]
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x07: // ADD A, [Y]
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x08: // ADC A, A
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x09: // ADC A, B
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x0A: // ADC A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x0B: // ADC A, [HL]
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x0C: // ADC A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x0D: // ADC A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x0E: // ADC A, [X]
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x0F: // ADC A, [Y]
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x10: // SUB A, A
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x11: // SUB A, B
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x12: // SUB A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x13: // SUB A, [HL]
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x14: // SUB A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x15: // SUB A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x16: // SUB A, [X]
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x17: // SUB A, [Y]
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x18: // SBC A, A
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x19: // SBC A, B
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x1A: // SBC A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x1B: // SBC A, [HL]
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x1C: // SBC A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x1D: // SBC A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x1E: // SBC A, [X]
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x1F: // SBC A, [Y]
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x20: // AND A, A
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x21: // AND A, B
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x22: // AND A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x23: // AND A, [HL]
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x24: // AND A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x25: // AND A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x26: // AND A, [X]
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x27: // AND A, [Y]
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x28: // OR A, A
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x29: // OR A, B
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x2A: // OR A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x2B: // OR A, [HL]
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x2C: // OR A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x2D: // OR A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x2E: // OR A, [X]
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x2F: // OR A, [Y]
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x30: // CMP A, A
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x31: // CMP A, B
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x32: // CMP A, #nn
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x33: // CMP A, [HL]
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x34: // CMP A, [N+#nn]
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x35: // CMP A, [#nnnn]
			I16 = Fetch16();
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x36: // CMP A, [X]
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x37: // CMP A, [Y]
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x38: // XOR A, A
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU.BA.B.L);
			return 8;
		case 0x39: // XOR A, B
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x3A: // XOR A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x3B: // XOR A, [HL]
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 8;
		case 0x3C: // XOR A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 12;
		case 0x3D: // XOR A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16));
			return 16;
		case 0x3E: // XOR A, [X]
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 8;
		case 0x3F: // XOR A, [Y]
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 8;

		case 0x40: // MOV A, A
			return 4;
		case 0x41: // MOV A, B
			MinxCPU.BA.B.L = MinxCPU.BA.B.H;
			return 4;
		case 0x42: // MOV A, L
			MinxCPU.BA.B.L = MinxCPU.HL.B.L;
			return 4;
		case 0x43: // MOV A, H
			MinxCPU.BA.B.L = MinxCPU.HL.B.H;
			return 4;
		case 0x44: // MOV A, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, MinxCPU.N.D + I8A);
			return 12;
		case 0x45: // MOV A, [HL]
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, MinxCPU.HL.D);
			return 8;
		case 0x46: // MOV A, [X]
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, MinxCPU.X.D);
			return 8;
		case 0x47: // MOV A, [Y]
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, MinxCPU.Y.D);
			return 8;

		case 0x48: // MOV B, A
			MinxCPU.BA.B.H = MinxCPU.BA.B.L;
			return 4;
		case 0x49: // MOV B, B
			return 4;
		case 0x4A: // MOV B, L
			MinxCPU.BA.B.H = MinxCPU.HL.B.L;
			return 4;
		case 0x4B: // MOV B, H
			MinxCPU.BA.B.H = MinxCPU.HL.B.H;
			return 4;
		case 0x4C: // MOV B, [N+#nn]
			I8A = Fetch8();
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, MinxCPU.N.D + I8A);
			return 12;
		case 0x4D: // MOV B, [HL]
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, MinxCPU.HL.D);
			return 8;
		case 0x4E: // MOV B, [X]
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, MinxCPU.X.D);
			return 8;
		case 0x4F: // MOV B, [Y]
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, MinxCPU.Y.D);
			return 8;

		case 0x50: // MOV L, A
			MinxCPU.HL.B.L = MinxCPU.BA.B.L;
			return 4;
		case 0x51: // MOV L, B
			MinxCPU.HL.B.L = MinxCPU.BA.B.H;
			return 4;
		case 0x52: // MOV L, L
			return 4;
		case 0x53: // MOV L, H
			MinxCPU.HL.B.L = MinxCPU.HL.B.H;
			return 4;
		case 0x54: // MOV L, [N+#nn]
			I8A = Fetch8();
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, MinxCPU.N.D + I8A);
			return 12;
		case 0x55: // MOV L, [HL]
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, MinxCPU.HL.D);
			return 8;
		case 0x56: // MOV L, [X]
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, MinxCPU.X.D);
			return 8;
		case 0x57: // MOV L, [Y]
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, MinxCPU.Y.D);
			return 8;

		case 0x58: // MOV H, A
			MinxCPU.HL.B.H = MinxCPU.BA.B.L;
			return 4;
		case 0x59: // MOV H, B
			MinxCPU.HL.B.H = MinxCPU.BA.B.H;
			return 4;
		case 0x5A: // MOV H, L
			MinxCPU.HL.B.H = MinxCPU.HL.B.L;
			return 4;
		case 0x5B: // MOV H, H
			return 4;
		case 0x5C: // MOV H, [N+#nn]
			I8A = Fetch8();
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, MinxCPU.N.D + I8A);
			return 12;
		case 0x5D: // MOV H, [HL]
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, MinxCPU.HL.D);
			return 8;
		case 0x5E: // MOV H, [X]
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, MinxCPU.X.D);
			return 8;
		case 0x5F: // MOV H, [Y]
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, MinxCPU.Y.D);
			return 8;

		case 0x60: // MOV [X], A
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU.BA.B.L);
			return 8;
		case 0x61: // MOV [X], B
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU.BA.B.H);
			return 8;
		case 0x62: // MOV [X], L
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU.HL.B.L);
			return 8;
		case 0x63: // MOV [X], H
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU.HL.B.H);
			return 8;
		case 0x64: // MOV [X], [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 16;
		case 0x65: // MOV [X], [HL]
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 12;
		case 0x66: // MOV [X], [X]
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 12;
		case 0x67: // MOV [X], [Y]
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 12;

		case 0x68: // MOV [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU.BA.B.L);
			return 8;
		case 0x69: // MOV [HL], B
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU.BA.B.H);
			return 8;
		case 0x6A: // MOV [HL], L
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU.HL.B.L);
			return 8;
		case 0x6B: // MOV [HL], H
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU.HL.B.H);
			return 8;
		case 0x6C: // MOV [HL], [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 16;
		case 0x6D: // MOV [HL], [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 12;
		case 0x6E: // MOV [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 12;
		case 0x6F: // MOV [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 12;

		case 0x70: // MOV [Y], A
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU.BA.B.L);
			return 8;
		case 0x71: // MOV [Y], B
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU.BA.B.H);
			return 8;
		case 0x72: // MOV [Y], L
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU.HL.B.L);
			return 8;
		case 0x73: // MOV [Y], H
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU.HL.B.H);
			return 8;
		case 0x74: // MOV [Y], [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, MinxCPU.N.D + I8A));
			return 16;
		case 0x75: // MOV [Y], [HL]
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 12;
		case 0x76: // MOV [Y], [X]
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 12;
		case 0x77: // MOV [Y], [Y]
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 12;

		case 0x78: // MOV [N+#nn], A
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU.BA.B.L);
			return 8;
		case 0x79: // MOV [N+#nn], B
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU.BA.B.H);
			return 8;
		case 0x7A: // MOV [N+#nn], L
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU.HL.B.L);
			return 8;
		case 0x7B: // MOV [N+#nn], H
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU.HL.B.H);
			return 8;
		case 0x7C: // NOTHING #nn
			I8A = Fetch8();
			return 64;
		case 0x7D: // MOV [N+#nn], [HL]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_OnRead(1, MinxCPU.HL.D));
			return 16;
		case 0x7E: // MOV [N+#nn], [X]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_OnRead(1, MinxCPU.X.D));
			return 16;
		case 0x7F: // MOV [N+#nn], [Y]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 16;

		case 0x80: // INC A
			MinxCPU.BA.B.L = MinxCPU_INC8(MinxCPU.BA.B.L);
			return 8;
		case 0x81: // INC B
			MinxCPU.BA.B.H = MinxCPU_INC8(MinxCPU.BA.B.H);
			return 8;
		case 0x82: // INC L
			MinxCPU.HL.B.L = MinxCPU_INC8(MinxCPU.HL.B.L);
			return 8;
		case 0x83: // INC H
			MinxCPU.HL.B.H = MinxCPU_INC8(MinxCPU.HL.B.H);
			return 8;
		case 0x84: // INC N
			MinxCPU.N.B.H = MinxCPU_INC8(MinxCPU.N.B.H);
			return 8;
		case 0x85: // INC [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_INC8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A)));
			return 16;
		case 0x86: // INC [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_INC8(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 12;
		case 0x87: // INC SP
			MinxCPU.SP.W.L = MinxCPU_INC16(MinxCPU.SP.W.L);
			return 8;

		case 0x88: // DEC A
			MinxCPU.BA.B.L = MinxCPU_DEC8(MinxCPU.BA.B.L);
			return 8;
		case 0x89: // DEC B
			MinxCPU.BA.B.H = MinxCPU_DEC8(MinxCPU.BA.B.H);
			return 8;
		case 0x8A: // DEC L
			MinxCPU.HL.B.L = MinxCPU_DEC8(MinxCPU.HL.B.L);
			return 8;
		case 0x8B: // DEC H
			MinxCPU.HL.B.H = MinxCPU_DEC8(MinxCPU.HL.B.H);
			return 8;
		case 0x8C: // DEC N
			MinxCPU.N.B.H = MinxCPU_DEC8(MinxCPU.N.B.H);
			return 8;
		case 0x8D: // DEC [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_DEC8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A)));
			return 16;
		case 0x8E: // DEC [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_DEC8(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 12;
		case 0x8F: // DEC SP
			MinxCPU.SP.W.L = MinxCPU_DEC16(MinxCPU.SP.W.L);
			return 8;

		case 0x90: // INC BA
			MinxCPU.BA.W.L = MinxCPU_INC16(MinxCPU.BA.W.L);
			return 8;
		case 0x91: // INC HL
			MinxCPU.HL.W.L = MinxCPU_INC16(MinxCPU.HL.W.L);
			return 8;
		case 0x92: // INC X
			MinxCPU.X.W.L = MinxCPU_INC16(MinxCPU.X.W.L);
			return 8;
		case 0x93: // INC Y
			MinxCPU.Y.W.L = MinxCPU_INC16(MinxCPU.Y.W.L);
			return 8;

		case 0x94: // TST A, B
			MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU.BA.B.H);
			return 8;
		case 0x95: // TST [HL], #nn
			I8A = Fetch8();
			MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A);
			return 12;
		case 0x96: // TST A, #nn
			I8A = Fetch8();
			MinxCPU_AND8(MinxCPU.BA.B.L, I8A);
			return 8;
		case 0x97: // TST B, #nn
			I8A = Fetch8();
			MinxCPU_AND8(MinxCPU.BA.B.H, I8A);
			return 8;

		case 0x98: // DEC BA
			MinxCPU.BA.W.L = MinxCPU_DEC16(MinxCPU.BA.W.L);
			return 8;
		case 0x99: // DEC HL
			MinxCPU.HL.W.L = MinxCPU_DEC16(MinxCPU.HL.W.L);
			return 8;
		case 0x9A: // DEC X
			MinxCPU.X.W.L = MinxCPU_DEC16(MinxCPU.X.W.L);
			return 8;
		case 0x9B: // DEC Y
			MinxCPU.Y.W.L = MinxCPU_DEC16(MinxCPU.Y.W.L);
			return 8;

		case 0x9C: // AND F, #nn
			I8A = Fetch8();
			MinxCPU.F = MinxCPU.F & I8A;
			MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
			return 12;
		case 0x9D: // OR F, #nn
			I8A = Fetch8();
			MinxCPU.F = MinxCPU.F | I8A;
			MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
			return 12;
		case 0x9E: // XOR F, #nn
			I8A = Fetch8();
			MinxCPU.F = MinxCPU.F ^ I8A;
			MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
			return 12;
		case 0x9F: // MOV F, #nn
			I8A = Fetch8();
			MinxCPU.F = I8A;
			MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
			return 12;

		case 0xA0: // PUSH BA
			MinxCPU_PUSH(MinxCPU.BA.B.H);
			MinxCPU_PUSH(MinxCPU.BA.B.L);
			return 16;
		case 0xA1: // PUSH HL
			MinxCPU_PUSH(MinxCPU.HL.B.H);
			MinxCPU_PUSH(MinxCPU.HL.B.L);
			return 16;
		case 0xA2: // PUSH X
			MinxCPU_PUSH(MinxCPU.X.B.H);
			MinxCPU_PUSH(MinxCPU.X.B.L);
			return 16;
		case 0xA3: // PUSH Y
			MinxCPU_PUSH(MinxCPU.Y.B.H);
			MinxCPU_PUSH(MinxCPU.Y.B.L);
			return 16;
		case 0xA4: // PUSH N
			MinxCPU_PUSH(MinxCPU.N.B.H);
			return 12;
		case 0xA5: // PUSH I
			MinxCPU_PUSH(MinxCPU.HL.B.I);
			return 12;
		case 0xA6: // PUSHX
			MinxCPU_PUSH(MinxCPU.X.B.I);
			MinxCPU_PUSH(MinxCPU.Y.B.I);
			return 16;
		case 0xA7: // PUSH F
			MinxCPU_PUSH(MinxCPU.F);
			return 12;

		case 0xA8: // POP BA
			MinxCPU.BA.B.L = MinxCPU_POP();
			MinxCPU.BA.B.H = MinxCPU_POP();
			return 12;
		case 0xA9: // POP HL
			MinxCPU.HL.B.L = MinxCPU_POP();
			MinxCPU.HL.B.H = MinxCPU_POP();
			return 12;
		case 0xAA: // POP X
			MinxCPU.X.B.L = MinxCPU_POP();
			MinxCPU.X.B.H = MinxCPU_POP();
			return 12;
		case 0xAB: // POP Y
			MinxCPU.Y.B.L = MinxCPU_POP();
			MinxCPU.Y.B.H = MinxCPU_POP();
			return 12;
		case 0xAC: // POP N
			MinxCPU.N.B.H = MinxCPU_POP();
			return 8;
		case 0xAD: // POP I
			MinxCPU.HL.B.I = MinxCPU_POP();
			MinxCPU.N.B.I = MinxCPU.HL.B.I;
			return 8;
		case 0xAE: // POPX
			MinxCPU.Y.B.I = MinxCPU_POP();
			MinxCPU.X.B.I = MinxCPU_POP();
			return 12;
		case 0xAF: // POP F
			MinxCPU.F = MinxCPU_POP();
			MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
			return 8;

		case 0xB0: // MOV A, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.L = I8A;
			return 8;
		case 0xB1: // MOV B, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.H = I8A;
			return 8;
		case 0xB2: // MOV L, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.L = I8A;
			return 8;
		case 0xB3: // MOV H, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.H = I8A;
			return 8;
		case 0xB4: // MOV N, #nn
			I8A = Fetch8();
			MinxCPU.N.B.H = I8A;
			return 8;
		case 0xB5: // MOV [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, I8A);
			return 12;
		case 0xB6: // MOV [X], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.X.D, I8A);
			return 12;
		case 0xB7: // MOV [Y], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.Y.D, I8A);
			return 12;

		case 0xB8: // MOV BA, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xB9: // MOV HL, [#nnnn]
			I16 = Fetch16();
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xBA: // MOV X, [#nnnn]
			I16 = Fetch16();
			MinxCPU.X.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.X.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xBB: // MOV Y, [#nnnn]
			I16 = Fetch16();
			MinxCPU.Y.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.Y.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;

		case 0xBC: // MOV [#nnnn], BA
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.BA.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 20;
		case 0xBD: // MOV [#nnnn], HL
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 20;
		case 0xBE: // MOV [#nnnn], X
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.X.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.X.B.H);
			return 20;
		case 0xBF: // MOV [#nnnn], Y
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.Y.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.Y.B.H);
			return 20;

		case 0xC0: // ADD BA, #nnnn
			I16 = Fetch16();
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, I16);
			return 12;
		case 0xC1: // ADD HL, #nnnn
			I16 = Fetch16();
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, I16);
			return 12;
		case 0xC2: // ADD X, #nnnn
			I16 = Fetch16();
			MinxCPU.X.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, I16);
			return 12;
		case 0xC3: // ADD Y, #nnnn
			I16 = Fetch16();
			MinxCPU.Y.W.L = MinxCPU_ADD16(MinxCPU.Y.W.L, I16);
			return 12;

		case 0xC4: // MOV BA, #nnnn
			I16 = Fetch16();
			MinxCPU.BA.W.L = I16;
			return 12;
		case 0xC5: // MOV HL, #nnnn
			I16 = Fetch16();
			MinxCPU.HL.W.L = I16;
			return 12;
		case 0xC6: // MOV X, #nnnn
			I16 = Fetch16();
			MinxCPU.X.W.L = I16;
			return 12;
		case 0xC7: // MOV Y, #nnnn
			I16 = Fetch16();
			MinxCPU.Y.W.L = I16;
			return 12;

		case 0xC8: // XCHG BA, HL
			I16 = MinxCPU.HL.W.L;
			MinxCPU.HL.W.L = MinxCPU.BA.W.L;
			MinxCPU.BA.W.L = I16;
			return 12;
		case 0xC9: // XCHG BA, X
			I16 = MinxCPU.X.W.L;
			MinxCPU.X.W.L = MinxCPU.BA.W.L;
			MinxCPU.BA.W.L = I16;
			return 12;
		case 0xCA: // XCHG BA, Y
			I16 = MinxCPU.Y.W.L;
			MinxCPU.Y.W.L = MinxCPU.BA.W.L;
			MinxCPU.BA.W.L = I16;
			return 12;
		case 0xCB: // XCHG BA, SP
			I16 = MinxCPU.SP.W.L;
			MinxCPU.SP.W.L = MinxCPU.BA.W.L;
			MinxCPU.BA.W.L = I16;
			return 12;

		case 0xCC: // XCHG A, B
			I8A = MinxCPU.BA.B.H;
			MinxCPU.BA.B.H = MinxCPU.BA.B.L;
			MinxCPU.BA.B.L = I8A;
			return 8;
		case 0xCD: // XCHG A, [HL]
			I8A = MinxCPU_OnRead(1, MinxCPU.HL.D);
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU.BA.B.L);
			MinxCPU.BA.B.L = I8A;
			return 12;

		case 0xCE: // Expand 0
			return MinxCPU_ExecCE();

		case 0xCF: // Expand 1
			return MinxCPU_ExecCF();

		case 0xD0: // SUB BA, #nnnn
			I16 = Fetch16();
			MinxCPU.BA.W.L = MinxCPU_SUB16(MinxCPU.BA.W.L, I16);
			return 12;
		case 0xD1: // SUB HL, #nnnn
			I16 = Fetch16();
			MinxCPU.HL.W.L = MinxCPU_SUB16(MinxCPU.HL.W.L, I16);
			return 12;
		case 0xD2: // SUB X, #nnnn
			I16 = Fetch16();
			MinxCPU.X.W.L = MinxCPU_SUB16(MinxCPU.X.W.L, I16);
			return 12;
		case 0xD3: // SUB Y, #nnnn
			I16 = Fetch16();
			MinxCPU.Y.W.L = MinxCPU_SUB16(MinxCPU.Y.W.L, I16);
			return 12;

		case 0xD4: // CMP BA, #nnnn
			I16 = Fetch16();
			MinxCPU_SUB16(MinxCPU.BA.W.L, I16);
			return 12;
		case 0xD5: // CMP HL, #nnnn
			I16 = Fetch16();
			MinxCPU_SUB16(MinxCPU.HL.W.L, I16);
			return 12;
		case 0xD6: // CMP X, #nnnn
			I16 = Fetch16();
			MinxCPU_SUB16(MinxCPU.X.W.L, I16);
			return 12;
		case 0xD7: // CMP Y, #nnnn
			I16 = Fetch16();
			MinxCPU_SUB16(MinxCPU.Y.W.L, I16);
			return 12;

		case 0xD8: // AND [N+#nn], #nn
			I8A = Fetch8();
			I8B = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A), I8B));
			return 20;
		case 0xD9: // OR [N+#nn], #nn
			I8A = Fetch8();
			I8B = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_OR8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A), I8B));
			return 20;
		case 0xDA: // XOR [N+#nn], #nn
			I8A = Fetch8();
			I8B = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, MinxCPU_XOR8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A), I8B));
			return 20;
		case 0xDB: // CMP [N+#nn], #nn
			I8A = Fetch8();
			I8B = Fetch8();
			MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A), I8B);
			return 16;
		case 0xDC: // TST [N+#nn], #nn
			I8A = Fetch8();
			I8B = Fetch8();
			MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.N.D + I8A), I8B);
			return 16;
		case 0xDD: // MOV [N+#nn], #nn
			I8A = Fetch8();
			I8B = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D + I8A, I8B);
			return 16;

		case 0xDE: // PACK
			MinxCPU.BA.B.L = (MinxCPU.BA.B.L & 0x0F) | (MinxCPU.BA.B.H << 4);
			return 8;

		case 0xDF: // UNPACK
			MinxCPU.BA.B.H = (MinxCPU.BA.B.L >> 4);
			MinxCPU.BA.B.L = MinxCPU.BA.B.L & 0x0F;
			return 8;

		case 0xE0: // CALLC #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_CARRY) {
				MinxCPU_CALLS(S8_TO_16(I8A));
				return 20;
			}
			return 8;
		case 0xE1: // CALLNC #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
				return 20;
			}
			return 8;
		case 0xE2: // CALLZ #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_ZERO) {
				MinxCPU_CALLS(S8_TO_16(I8A));
				return 20;
			}
			return 8;
		case 0xE3: // CALLNZ #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_ZERO)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
				return 20;
			}
			return 8;

		case 0xE4: // JC #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_CARRY) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 8;
		case 0xE5: // JNC #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 8;
		case 0xE6: // JZ #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_ZERO) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 8;
		case 0xE7: // JNZ #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_ZERO)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 8;

		case 0xE8: // CALLC #ssss
			I16 = Fetch16();
			if (MinxCPU.F & MINX_FLAG_CARRY) {
				MinxCPU_CALLS(I16);
				return 24;
			}
			return 12;
		case 0xE9: // CALLNC #ssss
			I16 = Fetch16();
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) {
				MinxCPU_CALLS(I16);
				return 24;
			}
			return 12;
		case 0xEA: // CALLZ #ssss
			I16 = Fetch16();
			if (MinxCPU.F & MINX_FLAG_ZERO) {
				MinxCPU_CALLS(I16);
				return 24;
			}
			return 12;
		case 0xEB: // CALLNZ #ssss
			I16 = Fetch16();
			if (!(MinxCPU.F & MINX_FLAG_ZERO)) {
				MinxCPU_CALLS(I16);
				return 24;
			}
			return 12;

		case 0xEC: // JC #ssss
			I16 = Fetch16();
			if (MinxCPU.F & MINX_FLAG_CARRY) {
				MinxCPU_JMPS(I16);
			}
			return 12;
		case 0xED: // JNC #ssss
			I16 = Fetch16();
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) {
				MinxCPU_JMPS(I16);
			}
			return 12;
		case 0xEE: // JZ #ssss
			I16 = Fetch16();
			if (MinxCPU.F & MINX_FLAG_ZERO) {
				MinxCPU_JMPS(I16);
			}
			return 12;
		case 0xEF: // JNZ #ssss
			I16 = Fetch16();
			if (!(MinxCPU.F & MINX_FLAG_ZERO)) {
				MinxCPU_JMPS(I16);
			}
			return 12;

		case 0xF0: // CALL #ss
			I8A = Fetch8();
			MinxCPU_CALLS(S8_TO_16(I8A));
			return 20;
		case 0xF1: // JMP #ss
			I8A = Fetch8();
			MinxCPU_JMPS(S8_TO_16(I8A));
			return 8;
		case 0xF2: // CALL #ssss
			I16 = Fetch16();
			MinxCPU_CALLS(I16);
			return 24;
		case 0xF3: // JMP #ssss
			I16 = Fetch16();
			MinxCPU_JMPS(I16);
			return 12;

		case 0xF4: // JMP HL
			MinxCPU_JMPU(MinxCPU.HL.W.L);
			return 8;

		case 0xF5: // JDBNZ #ss
			I8A = Fetch8();
			MinxCPU_JDBNZ(S8_TO_16(I8A));
			return 16;

		case 0xF6: // SWAP A
			MinxCPU.BA.B.L = MinxCPU_SWAP(MinxCPU.BA.B.L);
			return 8;
		case 0xF7: // SWAP [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SWAP(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 12;

		case 0xF8: // RET
			MinxCPU_RET();
			return 16;
		case 0xF9: // RETI
			MinxCPU_RETI();
			return 16;
		case 0xFA: // RETSKIP
			MinxCPU_RET();
			MinxCPU.PC.W.L = MinxCPU.PC.W.L + 2;
			return 16;

		case 0xFB: // CALL [#nnnn]
			I16 = Fetch16();
			MinxCPU_CALLX(I16);
			return 20;
		case 0xFC: // CINT #nn
			I16 = Fetch8();
			MinxCPU_CALLI(I16);
			return 20;
		case 0xFD: // JINT #nn
			I16 = Fetch8();
			MinxCPU_JMPI(I16);
			return 8;

		case 0xFE: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0xFE);
			return 4;

		case 0xFF: // NOP
			return 8;

		default:
			MinxCPU_OnException(EXCEPTION_UNKNOWN_INSTRUCTION, MinxCPU.IR);
			return 4;
	}
}

THUMB_CODE
int MinxCPU_ExecCE(void)
{
	uint8_t I8A;
	uint16_t I16;

	// Read IR
	MinxCPU.IR = Fetch8();

	// Process instruction
	switch(MinxCPU.IR) {

		case 0x00: // ADD A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x01: // ADD A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x02: // ADD A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x03: // ADD A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_ADD8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x04: // ADD [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADD8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x05: // ADD [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADD8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x06: // ADD [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADD8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x07: // ADD [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADD8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x08: // ADC A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x09: // ADC A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x0A: // ADC A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x0B: // ADC A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_ADC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x0C: // ADC [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADC8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x0D: // ADC [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADC8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x0E: // ADC [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADC8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x0F: // ADC [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ADC8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x10: // SUB A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x11: // SUB A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x12: // SUB A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x13: // SUB A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x14: // SUB [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x15: // SUB [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x16: // SUB [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x17: // SUB [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x18: // SBC A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x19: // SBC A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x1A: // SBC A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x1B: // SBC A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_SBC8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x1C: // SBC [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SBC8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x1D: // SBC [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SBC8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x1E: // SBC [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SBC8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x1F: // SBC [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SBC8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x20: // AND A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x21: // AND A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x22: // AND A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x23: // AND A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_AND8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x24: // AND [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x25: // AND [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x26: // AND [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x27: // AND [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_AND8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x28: // OR A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x29: // OR A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x2A: // OR A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x2B: // OR A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_OR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x2C: // OR [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OR8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x2D: // OR [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OR8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x2E: // OR [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OR8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x2F: // OR [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OR8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x30: // CMP A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x31: // CMP A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x32: // CMP A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x33: // CMP A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_SUB8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x34: // CMP [HL], A
			MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L);
			return 16;
		case 0x35: // CMP [HL], #nn
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A);
			return 20;
		case 0x36: // CMP [HL], [X]
			MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D));
			return 20;
		case 0x37: // CMP [HL], [Y]
			MinxCPU_SUB8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D));
			return 20;

		case 0x38: // XOR A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x39: // XOR A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;
		case 0x3A: // XOR A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 16;
		case 0x3B: // XOR A, [Y+L]
			I16 = MinxCPU.Y.W.L+ S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_XOR8(MinxCPU.BA.B.L, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 16;

		case 0x3C: // XOR [HL], A
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_XOR8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU.BA.B.L));
			return 16;
		case 0x3D: // XOR [HL], #nn
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_XOR8(MinxCPU_OnRead(1, MinxCPU.HL.D), I8A));
			return 20;
		case 0x3E: // XOR [HL], [X]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_XOR8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.X.D)));
			return 20;
		case 0x3F: // XOR [HL], [Y]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_XOR8(MinxCPU_OnRead(1, MinxCPU.HL.D), MinxCPU_OnRead(1, MinxCPU.Y.D)));
			return 20;

		case 0x40: // MOV A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x41: // MOV A, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;
		case 0x42: // MOV A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x43: // MOV A, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;

		case 0x44: // MOV [X+#ss], A
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.BA.B.L);
			return 16;
		case 0x45: // MOV [Y+#ss], A
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.BA.B.L);
			return 16;
		case 0x46: // MOV [X+L], A
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.BA.B.L);
			return 16;
		case 0x47: // MOV [Y+L], A
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.BA.B.L);
			return 16;

		case 0x48: // MOV B, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x49: // MOV B, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;
		case 0x4A: // MOV B, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x4B: // MOV B, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;

		case 0x4C: // MOV [X+#ss], B
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 16;
		case 0x4D: // MOV [Y+#ss], B
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 16;
		case 0x4E: // MOV [X+L], B
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 16;
		case 0x4F: // MOV [Y+L], B
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 16;

		case 0x50: // MOV L, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x51: // MOV L, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;
		case 0x52: // MOV L, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x53: // MOV L, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;

		case 0x54: // MOV [X+#ss], L
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.HL.B.L);
			return 16;
		case 0x55: // MOV [Y+#ss], L
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.HL.B.L);
			return 16;
		case 0x56: // MOV [X+L], L
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.HL.B.L);
			return 16;
		case 0x57: // MOV [Y+L], L
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.HL.B.L);
			return 16;

		case 0x58: // MOV H, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x59: // MOV H, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;
		case 0x5A: // MOV H, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 16;
		case 0x5B: // MOV H, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 16;

		case 0x5C: // MOV [X+#ss], H
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 16;
		case 0x5D: // MOV [Y+#ss], H
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 16;
		case 0x5E: // MOV [X+L], H
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 16;
		case 0x5F: // MOV [Y+L], H
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 16;

		case 0x60: // MOV [HL], [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 20;
		case 0x61: // MOV [HL], [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 20;
		case 0x62: // MOV [HL], [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 20;
		case 0x63: // MOV [HL], [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 20;

		case 0x64: // *ADC BA, #nnnn
			I16 = Fetch16();
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, I16);
			return 16;
		case 0x65: // *ADC HL, #nnnn
			I16 = Fetch16();
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, I16);
			return 16;

		case 0x66: // ??? BA, #nn00+L
			I8A = Fetch8();
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, (I8A << 8) | MinxCPU.HL.B.L);
			return 24;
		case 0x67: // ??? HL, #nn00+L
			I8A = Fetch8();
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, (I8A << 8) | MinxCPU.HL.B.L);
			return 24;

		case 0x68: // MOV [X], [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 20;
		case 0x69: // MOV [X], [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 20;
		case 0x6A: // MOV [X], [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 20;
		case 0x6B: // MOV [X], [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, MinxCPU.X.D, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 20;

		case 0x6C: // *ADD SP, #nnnn
			I16 = Fetch8();
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, I16);

		case 0x6D: // ??? HL, #nn
			I8A = Fetch8();
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, ((I8A << 4) * 3) + ((I8A & 0x08) >> 3));
			MinxCPU.F &= ~MINX_FLAG_CARRY; // It seems that carry gets clear?
			return 40;
		case 0x6E: // ??? SP, #nn00+L
			I8A = Fetch8();
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, (I8A << 8) | MinxCPU.HL.B.L);
			return 16;
		case 0x6F: // ??? HL, L
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, ((MinxCPU.HL.B.L << 4) * 3) + ((MinxCPU.HL.B.L & 0x08) >> 3));
			MinxCPU.F &= ~MINX_FLAG_CARRY; // It seems that carry gets clear?
			return 40;

		case 0x70: // NOTHING
			MinxCPU.PC.W.L++;
			return 64;
		case 0x71: // NOTHING
			MinxCPU.PC.W.L++;
			return 64;
		case 0x72: // NOTHING
			return 64;
		case 0x73: // NOTHING
			return 64;

		case 0x74: // *MOV A, [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 64;
		case 0x75: // *MOV L, [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 64;
		case 0x76: // *MOV A, [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 64;
		case 0x77: // *MOV L, [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 64;

		case 0x78: // MOV [Y], [X+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.X.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 20;
		case 0x79: // MOV [Y], [Y+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.Y.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 20;
		case 0x7A: // MOV [Y], [X+L]
			I16 = MinxCPU.X.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16));
			return 20;
		case 0x7B: // MOV [Y], [Y+L]
			I16 = MinxCPU.Y.W.L + S8_TO_16(MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, MinxCPU.Y.D, MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16));
			return 20;

		case 0x7C: // NOTHING #nn
			MinxCPU.PC.W.L++;
			return 20;
		case 0x7D: // NOTHING #nn
			MinxCPU.PC.W.L++;
			return 16;
		case 0x7E: // NOTHING
			return 20;
		case 0x7F: // NOTHING
			return 16;

		case 0x80: // SAL A
			MinxCPU.BA.B.L = MinxCPU_SAL(MinxCPU.BA.B.L);
			return 12;
		case 0x81: // SAL B
			MinxCPU.BA.B.H = MinxCPU_SAL(MinxCPU.BA.B.H);
			return 12;
		case 0x82: // SAL [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_SAL(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x83: // SAL [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SAL(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x84: // SHL A
			MinxCPU.BA.B.L = MinxCPU_SHL(MinxCPU.BA.B.L);
			return 12;
		case 0x85: // SHL B
			MinxCPU.BA.B.H = MinxCPU_SHL(MinxCPU.BA.B.H);
			return 12;
		case 0x86: // SHL [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_SHL(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x87: // SHL [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SHL(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x88: // SAR A
			MinxCPU.BA.B.L = MinxCPU_SAR(MinxCPU.BA.B.L);
			return 12;
		case 0x89: // SAR B
			MinxCPU.BA.B.H = MinxCPU_SAR(MinxCPU.BA.B.H);
			return 12;
		case 0x8A: // SAR [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_SAR(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x8B: // SAR [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SAR(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x8C: // SHR A
			MinxCPU.BA.B.L = MinxCPU_SHR(MinxCPU.BA.B.L);
			return 12;
		case 0x8D: // SHR B
			MinxCPU.BA.B.H = MinxCPU_SHR(MinxCPU.BA.B.H);
			return 12;
		case 0x8E: // SHR [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_SHR(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x8F: // SHR [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_SHR(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x90: // ROLC A
			MinxCPU.BA.B.L = MinxCPU_ROLC(MinxCPU.BA.B.L);
			return 12;
		case 0x91: // ROLC B
			MinxCPU.BA.B.H = MinxCPU_ROLC(MinxCPU.BA.B.H);
			return 12;
		case 0x92: // ROLC [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_ROLC(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x93: // ROLC [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ROLC(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x94: // ROL A
			MinxCPU.BA.B.L = MinxCPU_ROL(MinxCPU.BA.B.L);
			return 12;
		case 0x95: // ROL B
			MinxCPU.BA.B.H = MinxCPU_ROL(MinxCPU.BA.B.H);
			return 12;
		case 0x96: // ROL [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_ROL(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x97: // ROL [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ROL(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x98: // RORC A
			MinxCPU.BA.B.L = MinxCPU_RORC(MinxCPU.BA.B.L);
			return 12;
		case 0x99: // RORC B
			MinxCPU.BA.B.H = MinxCPU_RORC(MinxCPU.BA.B.H);
			return 12;
		case 0x9A: // RORC [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_RORC(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x9B: // RORC [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_RORC(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0x9C: // ROR A
			MinxCPU.BA.B.L = MinxCPU_ROR(MinxCPU.BA.B.L);
			return 12;
		case 0x9D: // ROR B
			MinxCPU.BA.B.H = MinxCPU_ROR(MinxCPU.BA.B.H);
			return 12;
		case 0x9E: // ROR [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_ROR(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0x9F: // ROR [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_ROR(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0xA0: // NOT A
			MinxCPU.BA.B.L = MinxCPU_NOT(MinxCPU.BA.B.L);
			return 12;
		case 0xA1: // NOT B
			MinxCPU.BA.B.H = MinxCPU_NOT(MinxCPU.BA.B.H);
			return 12;
		case 0xA2: // NOT [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_NOT(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0xA3: // NOT [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_NOT(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0xA4: // NEG A
			MinxCPU.BA.B.L = MinxCPU_NEG(MinxCPU.BA.B.L);
			return 12;
		case 0xA5: // NEG B
			MinxCPU.BA.B.H = MinxCPU_NEG(MinxCPU.BA.B.H);
			return 12;
		case 0xA6: // NEG [N+#nn]
			I8A = Fetch8();
			MinxCPU_OnWrite(1, MinxCPU.N.D | I8A, MinxCPU_NEG(MinxCPU_OnRead(1, MinxCPU.N.D | I8A)));
			return 20;
		case 0xA7: // NEG [HL]
			MinxCPU_OnWrite(1, MinxCPU.HL.D, MinxCPU_NEG(MinxCPU_OnRead(1, MinxCPU.HL.D)));
			return 16;

		case 0xA8: // EX BA, A
			MinxCPU.BA.W.L = S8_TO_16(MinxCPU.BA.B.L);
			return 12;

		case 0xA9: // NOTHING
			return 8;
		case 0xAA: // NOTHING
			return 12;

		case 0xAB: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0xABCE);
			return 64;
		case 0xAC: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0xACCE);
			return 64;
		case 0xAD: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0xACCE);
			return 64;

		case 0xAE: // HALT
			MinxCPU_HALT();
			return 8;
		case 0xAF: // STOP
			MinxCPU_STOP();
			return 8;

		case 0xB0: // AND B, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.H = MinxCPU_AND8(MinxCPU.BA.B.H, I8A);
			return 12;
		case 0xB1: // AND L, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.L = MinxCPU_AND8(MinxCPU.HL.B.L, I8A);
			return 12;
		case 0xB2: // AND H, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.H = MinxCPU_AND8(MinxCPU.HL.B.H, I8A);
			return 12;
		case 0xB3: // MOV H, V
			MinxCPU.HL.B.H = MinxCPU.PC.B.I;
			return 12;

		case 0xB4: // OR B, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.H = MinxCPU_OR8(MinxCPU.BA.B.H, I8A);
			return 12;
		case 0xB5: // OR L, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.L = MinxCPU_OR8(MinxCPU.HL.B.L, I8A);
			return 12;
		case 0xB6: // OR H, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.H = MinxCPU_OR8(MinxCPU.HL.B.H, I8A);
			return 12;
		case 0xB7: // ??? X
			MinxCPU.X.B.H = MinxCPU.PC.B.I;
			return 12;

		case 0xB8: // XOR B, #nn
			I8A = Fetch8();
			MinxCPU.BA.B.H = MinxCPU_XOR8(MinxCPU.BA.B.H, I8A);
			return 12;
		case 0xB9: // XOR L, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.L = MinxCPU_XOR8(MinxCPU.HL.B.L, I8A);
			return 12;
		case 0xBA: // XOR H, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.H = MinxCPU_XOR8(MinxCPU.HL.B.H, I8A);
			return 12;
		case 0xBB: // ??? Y
			MinxCPU.Y.B.H = MinxCPU.PC.B.I;
			return 12;

		case 0xBC: // CMP B, #nn
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU.BA.B.H, I8A);
			return 12;
		case 0xBD: // CMP L, #nn
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU.HL.B.L, I8A);
			return 12;
		case 0xBE: // CMP H, #nn
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU.HL.B.H, I8A);
			return 12;
		case 0xBF: // CMP N, #nn
			I8A = Fetch8();
			MinxCPU_SUB8(MinxCPU.N.B.H, I8A);
			return 12;

		case 0xC0: // MOV A, N
			MinxCPU.BA.B.L = MinxCPU.N.B.H;
			return 8;
		case 0xC1: // MOV A, F
			MinxCPU.BA.B.L = MinxCPU.F;
			return 8;
		case 0xC2: // MOV N, A
			MinxCPU.N.B.H = MinxCPU.BA.B.L;
			return 8;
		case 0xC3: // MOV F, A
			MinxCPU.F = MinxCPU.BA.B.L;
			MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
			return 8;

		case 0xC4: // MOV U, #nn
			I8A = Fetch8();
			Set_U(I8A);
			return 16;
		case 0xC5: // MOV I, #nn
			I8A = Fetch8();
			MinxCPU.HL.B.I = I8A;
			MinxCPU.N.B.I = MinxCPU.HL.B.I;
			return 12;
		case 0xC6: // MOV XI, #nn
			I8A = Fetch8();
			MinxCPU.X.B.I = I8A;
			return 12;
		case 0xC7: // MOV YI, #nn
			I8A = Fetch8();
			MinxCPU.Y.B.I = I8A;
			return 12;

		case 0xC8: // MOV A, V
			MinxCPU.BA.B.L = MinxCPU.PC.B.I;
			return 8;
		case 0xC9: // MOV A, I
			MinxCPU.BA.B.L = MinxCPU.HL.B.I;
			return 8;
		case 0xCA: // MOV A, XI
			MinxCPU.BA.B.L = MinxCPU.X.B.I;
			return 8;
		case 0xCB: // MOV A, YI
			MinxCPU.BA.B.L = MinxCPU.Y.B.I;
			return 8;

		case 0xCC: // MOV U, A
			Set_U(MinxCPU.BA.B.L);
			return 8;
		case 0xCD: // MOV I, A
			MinxCPU.HL.B.I = MinxCPU.BA.B.L;
			MinxCPU.N.B.I = MinxCPU.HL.B.I;
			return 8;
		case 0xCE: // MOV XI, A
			MinxCPU.X.B.I = MinxCPU.BA.B.L;
			return 8;
		case 0xCF: // MOV YI, A
			MinxCPU.Y.B.I = MinxCPU.BA.B.L;
			return 8;

		case 0xD0: // MOV A, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xD1: // MOV B, [#nnnn]
			I16 = Fetch16();
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xD2: // MOV L, [#nnnn]
			I16 = Fetch16();
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xD3: // MOV H, [#nnnn]
			I16 = Fetch16();
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;

		case 0xD4: // MOV [#nnnn], A
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.BA.B.L);
			return 20;
		case 0xD5: // MOV [#nnnn], B
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 20;
		case 0xD6: // MOV [#nnnn], L
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.HL.B.L);
			return 20;
		case 0xD7: // MOV [#nnnn], H
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 20;

		case 0xD8: // MinxCPU_MUL L, A
			MinxCPU_MUL();
			return 48;

		case 0xD9: // DIV HL, A
			MinxCPU_DIV();
			return 52;

		case 0xDA: // ??? #nn
		case 0xDB: // ??? #nn
			return MinxCPU_ExecSPCE();
		case 0xDC: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0xDCCE);
			return 64;
		case 0xDD: // NOTHING
			return 16;
		case 0xDE: // ??? #nn
		case 0xDF: // ??? #nn
			return MinxCPU_ExecSPCE();

		case 0xE0: // JL #ss
			I8A = Fetch8();
			if ( ((MinxCPU.F & MINX_FLAG_OVERFLOW)!=0) != ((MinxCPU.F & MINX_FLAG_SIGN)!=0) ) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE1: // JLE #ss
			I8A = Fetch8();
			if ( (((MinxCPU.F & MINX_FLAG_OVERFLOW)==0) != ((MinxCPU.F & MINX_FLAG_SIGN)==0)) || ((MinxCPU.F & MINX_FLAG_ZERO)!=0) ) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE2: // JG #ss
			I8A = Fetch8();
			if ( (((MinxCPU.F & MINX_FLAG_OVERFLOW)!=0) == ((MinxCPU.F & MINX_FLAG_SIGN)!=0)) && ((MinxCPU.F & MINX_FLAG_ZERO)==0) ) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE3: // JGE #ss
			I8A = Fetch8();
			if ( ((MinxCPU.F & MINX_FLAG_OVERFLOW)==0) == ((MinxCPU.F & MINX_FLAG_SIGN)==0) ) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;

		case 0xE4: // JO #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_OVERFLOW) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE5: // JNO #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_OVERFLOW)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE6: // JP #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_SIGN)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE7: // JNP #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_SIGN) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;

		case 0xE8: // JNX0 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x01)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xE9: // JNX1 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x02)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xEA: // JNX2 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x04)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xEB: // JNX3 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x08)) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;

		case 0xEC: // JX0 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x01) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xED: // JX1 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x02) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xEE: // JX2 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x04) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;
		case 0xEF: // JX3 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x08) {
				MinxCPU_JMPS(S8_TO_16(I8A));
			}
			return 12;

		case 0xF0: // CALLL #ss
			I8A = Fetch8();
			if ( ((MinxCPU.F & MINX_FLAG_OVERFLOW)!=0) != ((MinxCPU.F & MINX_FLAG_SIGN)!=0) ) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF1: // CALLLE #ss
			I8A = Fetch8();
			if ( (((MinxCPU.F & MINX_FLAG_OVERFLOW)==0) != ((MinxCPU.F & MINX_FLAG_SIGN)==0)) || ((MinxCPU.F & MINX_FLAG_ZERO)!=0) ) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF2: // CALLG #ss
			I8A = Fetch8();
			if ( (((MinxCPU.F & MINX_FLAG_OVERFLOW)!=0) == ((MinxCPU.F & MINX_FLAG_SIGN)!=0)) && ((MinxCPU.F & MINX_FLAG_ZERO)==0) ) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF3: // CALLGE #ss
			I8A = Fetch8();
			if ( ((MinxCPU.F & MINX_FLAG_OVERFLOW)==0) == ((MinxCPU.F & MINX_FLAG_SIGN)==0) ) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;

		case 0xF4: // CALLO #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_OVERFLOW) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF5: // CALLNO #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_OVERFLOW)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF6: // CALLNS #ss
			I8A = Fetch8();
			if (!(MinxCPU.F & MINX_FLAG_SIGN)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF7: // CALLS #ss
			I8A = Fetch8();
			if (MinxCPU.F & MINX_FLAG_SIGN) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;

		case 0xF8: // CALLNX0 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x01)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xF9: // CALLNX1 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x02)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xFA: // CALLNX2 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x04)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xFB: // CALLNX3 #ss
			I8A = Fetch8();
			if (!(MinxCPU.E & 0x08)) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;

		case 0xFC: // CALLX0 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x01) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xFD: // CALLX1 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x02) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xFE: // CALLX2 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x04) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;
		case 0xFF: // CALLX3 #ss
			I8A = Fetch8();
			if (MinxCPU.E & 0x08) {
				MinxCPU_CALLS(S8_TO_16(I8A));
			}
			return 12;

		default:
			MinxCPU_OnException(EXCEPTION_UNKNOWN_INSTRUCTION, (MinxCPU.IR << 8) + 0xCE);
			return 4;
	}
}

THUMB_CODE
int MinxCPU_ExecSPCE(void)
{
	// Read IR
	MinxCPU.IR = Fetch8();

	// Process instruction
	switch(MinxCPU.IR) {
		// TODO!
		default:
			MinxCPU_OnException(EXCEPTION_UNKNOWN_INSTRUCTION, (MinxCPU.IR << 16) + 0xCEFF);
			return 64;
	}
}

THUMB_CODE
int MinxCPU_ExecSPCF(void)
{
	// Read IR
	MinxCPU.IR = Fetch8();

	// Process instruction
	switch(MinxCPU.IR) {
		case 0x00: case 0x01: case 0x02: case 0x03: // Decrement BA
			MinxCPU.BA.W.L = MinxCPU_DEC16(MinxCPU.BA.W.L);
			return 16;
		case 0x04: case 0x05: case 0x06: case 0x07: // Decrement BA if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.BA.W.L = MinxCPU_DEC16(MinxCPU.BA.W.L);
			return 16;
		case 0x08: case 0x09: case 0x0A: case 0x0B: // Increment BA
			MinxCPU.BA.W.L = MinxCPU_INC16(MinxCPU.BA.W.L);
			return 16;
		case 0x0C: case 0x0D: case 0x0E: case 0x0F: // Increment BA if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.BA.W.L = MinxCPU_INC16(MinxCPU.BA.W.L);
			return 16;
		case 0x10: case 0x11: case 0x12: case 0x13: // Decrement BA
			MinxCPU.BA.W.L = MinxCPU_DEC16(MinxCPU.BA.W.L);
			return 16;
		case 0x14: case 0x15: case 0x16: case 0x17: // Decrement BA if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.BA.W.L = MinxCPU_DEC16(MinxCPU.BA.W.L);
			return 16;
		case 0x18: case 0x19: case 0x1A: case 0x1B: // Increment BA (Doesn't save result!!)
			MinxCPU_INC16(MinxCPU.BA.W.L);
			return 16;
		case 0x1C: case 0x1D: case 0x1E: case 0x1F: // Increment BA if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.BA.W.L = MinxCPU_INC16(MinxCPU.BA.W.L);
			return 16;

		case 0x20: case 0x21: case 0x22: case 0x23: // Decrement HL
			MinxCPU.HL.W.L = MinxCPU_DEC16(MinxCPU.HL.W.L);
			return 16;
		case 0x24: case 0x25: case 0x26: case 0x27: // Decrement HL if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.HL.W.L = MinxCPU_DEC16(MinxCPU.HL.W.L);
			return 16;
		case 0x28: case 0x29: case 0x2A: case 0x2B: // Increment HL
			MinxCPU.HL.W.L = MinxCPU_INC16(MinxCPU.HL.W.L);
			return 16;
		case 0x2C: case 0x2D: case 0x2E: case 0x2F: // Increment HL if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.HL.W.L = MinxCPU_INC16(MinxCPU.HL.W.L);
			return 16;
		case 0x30: case 0x31: case 0x32: case 0x33: // Decrement HL
			MinxCPU.HL.W.L = MinxCPU_DEC16(MinxCPU.HL.W.L);
			return 16;
		case 0x34: case 0x35: case 0x36: case 0x37: // Decrement HL if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.HL.W.L = MinxCPU_DEC16(MinxCPU.HL.W.L);
			return 16;
		case 0x38: case 0x39: case 0x3A: case 0x3B: // Increment HL (Doesn't save result!!)
			MinxCPU_INC16(MinxCPU.HL.W.L);
			return 16;
		case 0x3C: case 0x3D: case 0x3E: case 0x3F: // Increment HL if Carry = 0
			if (!(MinxCPU.F & MINX_FLAG_CARRY)) MinxCPU.HL.W.L = MinxCPU_INC16(MinxCPU.HL.W.L);
			return 16;

		case 0x40: case 0x41: // Decrement X
			MinxCPU.X.W.L = MinxCPU_DEC16(MinxCPU.X.W.L);
			return 16;
		case 0x42: case 0x43: // Decrement Y
			MinxCPU.X.W.L = MinxCPU_DEC16(MinxCPU.X.W.L);
			return 16;
		case 0x44: case 0x45: // Decrement SP
			MinxCPU.SP.W.L = MinxCPU_DEC16(MinxCPU.SP.W.L);
			return 16;
		case 0x46: case 0x47: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 16;

		case 0x48: case 0x49: // Increment X
			MinxCPU.X.W.L = MinxCPU_INC16(MinxCPU.X.W.L);
			return 16;
		case 0x4A: case 0x4B: // Increment Y
			MinxCPU.X.W.L = MinxCPU_INC16(MinxCPU.X.W.L);
			return 16;
		case 0x4C: case 0x4D: // Increment SP
			MinxCPU.SP.W.L = MinxCPU_INC16(MinxCPU.SP.W.L);
			return 16;
		case 0x4E: case 0x4F: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 16;

		case 0x50: case 0x51: // Decrement X
			MinxCPU.X.W.L = MinxCPU_DEC16(MinxCPU.X.W.L);
			return 16;
		case 0x52: case 0x53: // Decrement Y
			MinxCPU.X.W.L = MinxCPU_DEC16(MinxCPU.X.W.L);
			return 16;
		case 0x54: case 0x55: // Decrement SP
			MinxCPU.SP.W.L = MinxCPU_DEC16(MinxCPU.SP.W.L);
			return 16;
		case 0x56: case 0x57: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 16;

		case 0x58: case 0x59: // Increment X
			MinxCPU.X.W.L = MinxCPU_INC16(MinxCPU.X.W.L);
			return 16;
		case 0x5A: case 0x5B: // Increment Y
			MinxCPU.X.W.L = MinxCPU_INC16(MinxCPU.X.W.L);
			return 16;
		case 0x5C: case 0x5D: // Increment SP
			MinxCPU.SP.W.L = MinxCPU_INC16(MinxCPU.SP.W.L);
			return 16;
		case 0x5E: case 0x5F: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 16;

		case 0x60: case 0x61: case 0x62: case 0x63: // CRASH
		case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6A: case 0x6B:
		case 0x6C: case 0x6D: case 0x6E: case 0x6F:
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 16;

		case 0x70: // BA = (0x4D << 8) | V
			MinxCPU.BA.W.L = 0x4D00 | MinxCPU.PC.B.I;
			return 24;
		case 0x71: // HL = (0x4D << 8) | V
			MinxCPU.HL.W.L = 0x4D00 | MinxCPU.PC.B.I;
			return 24;
		case 0x72: // X = (0x4D << 8) | V
			MinxCPU.X.W.L = 0x4D00 | MinxCPU.PC.B.I;
			return 24;
		case 0x73: // Y = (0x4D << 8) | V
			MinxCPU.Y.W.L = 0x4D00 | MinxCPU.PC.B.I;
			return 24;

		case 0x74: case 0x75: case 0x76: case 0x77: // NOTHING
			return 24;

		case 0x78: case 0x79: case 0x7A: case 0x7B: // CRASH
		case 0x7C: case 0x7D: case 0x7E: case 0x7F:
		case 0x80: case 0x81: case 0x82: case 0x83:
		case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x89: case 0x8A: case 0x8B:
		case 0x8C: case 0x8D: case 0x8E: case 0x8F:
		case 0x90: case 0x91: case 0x92: case 0x93:
		case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9A: case 0x9B:
		case 0x9C: case 0x9D: case 0x9E: case 0x9F:
		case 0xA0: case 0xA1: case 0xA2: case 0xA3:
		case 0xA4: case 0xA5: case 0xA6: case 0xA7:
		case 0xA8: case 0xA9: case 0xAA: case 0xAB:
		case 0xAC: case 0xAD: case 0xAE: case 0xAF:
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 24;

		case 0xB0: case 0xB1: case 0xB2: case 0xB3: // NOTHING
			return 12;

		case 0xB4: case 0xB5: case 0xB6: case 0xB7: // CRASH
		case 0xB8: case 0xB9: case 0xBA: case 0xBB:
		case 0xBC: case 0xBD: case 0xBE: case 0xBF:
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 12;

		case 0xC0: // BA = (0x20 << 8) | V
			MinxCPU.BA.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xC1: // HL = (0x20 << 8) | V
			MinxCPU.HL.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xC2: // X = (0x20 << 8) | V
			MinxCPU.X.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xC3: // Y = (0x20 << 8) | V
			MinxCPU.Y.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;

		case 0xC4: case 0xC5: case 0xC6: case 0xC7: // NOTHING
			return 20;

		case 0xC8: case 0xC9: case 0xCA: case 0xCB: // CRASH
		case 0xCC: case 0xCD: case 0xCE: case 0xCF:
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 20;

		case 0xD0: // BA = (0x20 << 8) | V
			MinxCPU.BA.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xD1: // HL = (0x20 << 8) | V
			MinxCPU.HL.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xD2: // X = (0x20 << 8) | V
			MinxCPU.X.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xD3: // Y = (0x00 << 8) | V ? (EH!?)
			MinxCPU.Y.W.L = 0x0000 | MinxCPU.PC.B.I;
			return 20;

		case 0xD4: case 0xD5: case 0xD6: case 0xD7: // NOTHING
			return 20;

		case 0xD8: // BA = (0x20 << 8) | V
			MinxCPU.BA.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xD9: // HL = (0x20 << 8) | V
			MinxCPU.HL.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xDA: // X = (0x20 << 8) | V
			MinxCPU.X.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;
		case 0xDB: // Y = (0x20 << 8) | V
			MinxCPU.Y.W.L = 0x2000 | MinxCPU.PC.B.I;
			return 20;

		case 0xDC: case 0xDD: case 0xDE: case 0xDF: // NOTHING
			return 20;

		case 0xE0: case 0xE1: case 0xE2: case 0xE3: // CRASH
		case 0xE4: case 0xE5: case 0xE6: case 0xE7:
		case 0xE8: case 0xE9: case 0xEA: case 0xEB:
		case 0xEC: case 0xED: case 0xEE: case 0xEF:
		case 0xF0: case 0xF1: case 0xF2: case 0xF3:
		case 0xF4: case 0xF5: case 0xF6: case 0xF7:
		case 0xF8: case 0xF9: case 0xFA: case 0xFB:
		case 0xFC: case 0xFD: case 0xFE: case 0xFF:
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, (MinxCPU.IR << 16) | 0xBFCF);
			return 24;

		default:
			MinxCPU_OnException(EXCEPTION_UNKNOWN_INSTRUCTION, (MinxCPU.IR << 16) + 0xBFCF);
			return 64;
	}
}

THUMB_CODE
int MinxCPU_ExecCF(void)
{
	uint8_t I8A;
	uint16_t I16;

	// Read IR
	MinxCPU.IR = Fetch8();

	// Process instruction
	switch(MinxCPU.IR) {
		case 0x00: // ADD BA, BA
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x01: // ADD BA, HL
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x02: // ADD BA, X
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x03: // ADD BA, Y
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x04: // ADC BA, BA
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x05: // ADC BA, HL
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x06: // ADC BA, X
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x07: // ADC BA, Y
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x08: // SUB BA, BA
			MinxCPU.BA.W.L = MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x09: // SUB BA, HL
			MinxCPU.BA.W.L = MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x0A: // SUB BA, X
			MinxCPU.BA.W.L = MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x0B: // SUB BA, Y
			MinxCPU.BA.W.L = MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x0C: // SBC BA, BA
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x0D: // SBC BA, HL
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x0E: // SBC BA, X
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x0F: // SBC BA, Y
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x10: // *ADD BA, BA
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x11: // *ADD BA, HL
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x12: // *ADD BA, X
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x13: // *ADD BA, Y
			MinxCPU.BA.W.L = MinxCPU_ADD16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x14: // *ADC BA, BA
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x15: // *ADC BA, HL
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x16: // *ADC BA, X
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x17: // *ADC BA, Y
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x18: // CMP BA, BA
			MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x19: // CMP BA, HL
			MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x1A: // CMP BA, X
			MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x1B: // CMP BA, Y
			MinxCPU_SUB16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x1C: // *SBC BA, BA
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x1D: // *SBC BA, HL
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x1E: // *SBC BA, X
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x1F: // *SBC BA, Y
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x20: // ADD HL, BA
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x21: // ADD HL, HL
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x22: // ADD HL, X
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x23: // ADD HL, Y
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x24: // ADC HL, BA
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x25: // ADC HL, HL
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x26: // ADC HL, X
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x27: // ADC HL, Y
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x28: // SUB HL, BA
			MinxCPU.HL.W.L = MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x29: // SUB HL, HL
			MinxCPU.HL.W.L = MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x2A: // SUB HL, X
			MinxCPU.HL.W.L = MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x2B: // SUB HL, Y
			MinxCPU.HL.W.L = MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x2C: // SBC HL, BA
			MinxCPU.HL.W.L = MinxCPU_SBC16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x2D: // SBC HL, HL
			MinxCPU.HL.W.L = MinxCPU_SBC16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x2E: // SBC HL, X
			MinxCPU.HL.W.L = MinxCPU_SBC16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x2F: // SBC HL, Y
			MinxCPU.HL.W.L = MinxCPU_SBC16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x30: // *ADD HL, BA
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x31: // *ADD BA, HL
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x32: // *ADD BA, X
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x33: // *ADD BA, Y
			MinxCPU.HL.W.L = MinxCPU_ADD16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x34: // *ADC HL, BA
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x35: // *ADC HL, HL
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x36: // *ADC HL, X
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x37: // *ADC HL, Y
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x38: // CMP HL, BA
			MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x39: // CMP HL, HL
			MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x3A: // CMP HL, X
			MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x3B: // CMP HL, Y
			MinxCPU_SUB16(MinxCPU.HL.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x3C: // *SBC BA, BA
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x3D: // *SBC BA, HL
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x3E: // *SBC BA, X
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.X.W.L);
			return 16;
		case 0x3F: // *SBC BA, Y
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, MinxCPU.Y.W.L);
			return 16;

		case 0x40: // ADD X, BA
			MinxCPU.X.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x41: // ADD X, HL
			MinxCPU.X.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x42: // ADD Y, BA
			MinxCPU.Y.W.L = MinxCPU_ADD16(MinxCPU.Y.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x43: // ADD Y, HL
			MinxCPU.Y.W.L = MinxCPU_ADD16(MinxCPU.Y.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x44: // ADD SP, BA
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x45: // ADD SP, HL
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x46: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x46CF);
			return 16;
		case 0x47: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x47CF);
			return 16;

		case 0x48: // SUB X, BA
			MinxCPU.X.W.L = MinxCPU_SUB16(MinxCPU.X.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x49: // SUB X, HL
			MinxCPU.X.W.L = MinxCPU_SUB16(MinxCPU.X.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x4A: // SUB Y, BA
			MinxCPU.Y.W.L = MinxCPU_SUB16(MinxCPU.Y.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x4B: // SUB Y, HL
			MinxCPU.Y.W.L = MinxCPU_SUB16(MinxCPU.Y.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x4C: // SUB SP, BA
			MinxCPU.SP.W.L = MinxCPU_SUB16(MinxCPU.SP.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x4D: // SUB SP, HL
			MinxCPU.SP.W.L = MinxCPU_SUB16(MinxCPU.SP.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x4E: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x4ECF);
			return 16;
		case 0x4F: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x4FCF);
			return 16;

		case 0x50: // *ADD X, BA
			MinxCPU.X.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x51: // *ADD X, HL
			MinxCPU.X.W.L = MinxCPU_ADD16(MinxCPU.X.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x52: // *ADD Y, BA
			MinxCPU.Y.W.L = MinxCPU_ADD16(MinxCPU.Y.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x53: // *ADD Y, HL
			MinxCPU.Y.W.L = MinxCPU_ADD16(MinxCPU.Y.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x54: // *ADD SP, BA
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x55: // *ADD SP, HL
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x56: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x56CF);
			return 16;
		case 0x57: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x57CF);
			return 16;

		case 0x58: // *SUB X, BA
			MinxCPU.X.W.L = MinxCPU_SUB16(MinxCPU.X.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x59: // *SUB X, HL
			MinxCPU.X.W.L = MinxCPU_SUB16(MinxCPU.X.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x5A: // *SUB Y, BA
			MinxCPU.Y.W.L = MinxCPU_SUB16(MinxCPU.Y.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x5B: // *SUB Y, HL
			MinxCPU.Y.W.L = MinxCPU_SUB16(MinxCPU.Y.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x5C: // CMP SP, BA
			MinxCPU_SUB16(MinxCPU.SP.W.L, MinxCPU.BA.W.L);
			return 16;
		case 0x5D: // CMP SP, HL
			MinxCPU_SUB16(MinxCPU.SP.W.L, MinxCPU.HL.W.L);
			return 16;
		case 0x5E: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x5ECF);
			return 16;
		case 0x5F: // CRASH
			MinxCPU_OnException(EXCEPTION_CRASH_INSTRUCTION, 0x5FCF);
			return 16;

		case 0x60: // ADC BA, #nnnn
			I16 = Fetch16();
			MinxCPU.BA.W.L = MinxCPU_ADC16(MinxCPU.BA.W.L, I16);
			return 16;
		case 0x61: // ADC HL, #nnnn
			I16 = Fetch16();
			MinxCPU.HL.W.L = MinxCPU_ADC16(MinxCPU.HL.W.L, I16);
			return 16;
		case 0x62: // SBC BA, #nnnn
			I16 = Fetch16();
			MinxCPU.BA.W.L = MinxCPU_SBC16(MinxCPU.BA.W.L, I16);
			return 16;
		case 0x63: // SBC HL, #nnnn
			I16 = Fetch16();
			MinxCPU.HL.W.L = MinxCPU_SBC16(MinxCPU.HL.W.L, I16);
			return 16;

		case 0x64: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x64CF);
			return 16;
		case 0x65: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x65CF);
			return 16;
		case 0x66: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x66CF);
			return 16;
		case 0x67: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x67CF);
			return 16;

		case 0x68: // ADD SP, #nnnn
			I16 = Fetch16();
			MinxCPU.SP.W.L = MinxCPU_ADD16(MinxCPU.SP.W.L, I16);
			return 16;
		case 0x69: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x69CF);
			return 16;

		case 0x6A: // SUB SP, #nnnn
			I16 = Fetch16();
			MinxCPU.SP.W.L = MinxCPU_SUB16(MinxCPU.SP.W.L, I16);
			return 16;
		case 0x6B: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x6BCF);
			return 16;

		case 0x6C: // CMP SP, #nnnn
			I16 = Fetch16();
			MinxCPU_SUB16(MinxCPU.SP.W.L, I16);
			return 16;
		case 0x6D: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x6DCF);
			return 16;

		case 0x6E: // MOV SP, #nnnn
			I16 = Fetch16();
			MinxCPU.SP.W.L = I16;
			return 16;
		case 0x6F: // UNSTABLE
			MinxCPU_OnException(EXCEPTION_UNSTABLE_INSTRUCTION, 0x6FCF);
			return 16;

		case 0x70: // MOV BA, [SP+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, I16++);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, I16);
			return 24;
		case 0x71: // MOV HL, [SP+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, I16++);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, I16);
			return 24;
		case 0x72: // MOV X, [SP+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU.X.B.L = MinxCPU_OnRead(1, I16++);
			MinxCPU.X.B.H = MinxCPU_OnRead(1, I16);
			return 24;
		case 0x73: // MOV Y, [SP+#ss]
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU.Y.B.L = MinxCPU_OnRead(1, I16++);
			MinxCPU.Y.B.H = MinxCPU_OnRead(1, I16);
			return 24;

		case 0x74: // MOV [SP+#ss], BA
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, I16++, MinxCPU.BA.B.L);
			MinxCPU_OnWrite(1, I16, MinxCPU.BA.B.H);
			return 24;
		case 0x75: // MOV [SP+#ss], HL
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, I16++, MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, I16, MinxCPU.HL.B.H);
			return 24;
		case 0x76: // MOV [SP+#ss], X
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, I16++, MinxCPU.X.B.L);
			MinxCPU_OnWrite(1, I16, MinxCPU.X.B.H);
			return 24;
		case 0x77: // MOV [SP+#ss], Y
			I8A = Fetch8();
			I16 = MinxCPU.SP.W.L + S8_TO_16(I8A);
			MinxCPU_OnWrite(1, I16++, MinxCPU.Y.B.L);
			MinxCPU_OnWrite(1, I16, MinxCPU.Y.B.H);
			return 24;

		case 0x78: // MOV SP, [#nnnn]
			I16 = Fetch16();
			MinxCPU.SP.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.SP.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 24;

		case 0x79: // ??? #nn
		case 0x7A: // ??? #nn
		case 0x7B: // ??? #nn
			return MinxCPU_ExecSPCF();

		case 0x7C: // MOV [#nnnn], SP
			I16 = Fetch16();
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.SP.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.SP.B.H);
			return 24;

		case 0x7D: case 0x7E: case 0x7F: // ??? #nn
		case 0x80: case 0x81: case 0x82: case 0x83:
		case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x89: case 0x8A: case 0x8B:
		case 0x8C: case 0x8D: case 0x8E: case 0x8F:
		case 0x90: case 0x91: case 0x92: case 0x93:
		case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9A: case 0x9B:
		case 0x9C: case 0x9D: case 0x9E: case 0x9F:
		case 0xA0: case 0xA1: case 0xA2: case 0xA3:
		case 0xA4: case 0xA5: case 0xA6: case 0xA7:
		case 0xA8: case 0xA9: case 0xAA: case 0xAB:
		case 0xAC: case 0xAD: case 0xAE: case 0xAF:
			return MinxCPU_ExecSPCF();

		case 0xB0: // PUSH A
			MinxCPU_PUSH(MinxCPU.BA.B.L);
			return 12;
		case 0xB1: // PUSH B
			MinxCPU_PUSH(MinxCPU.BA.B.H);
			return 12;
		case 0xB2: // PUSH L
			MinxCPU_PUSH(MinxCPU.HL.B.L);
			return 12;
		case 0xB3: // PUSH H
			MinxCPU_PUSH(MinxCPU.HL.B.H);
			return 12;

		case 0xB4: // POP A
			MinxCPU.BA.B.L = MinxCPU_POP();
			return 12;
		case 0xB5: // POP B
			MinxCPU.BA.B.H = MinxCPU_POP();
			return 12;
		case 0xB6: // POP L
			MinxCPU.HL.B.L = MinxCPU_POP();
			return 12;
		case 0xB7: // POP H
			MinxCPU.HL.B.H = MinxCPU_POP();
			return 12;
		case 0xB8: // PUSHA
			MinxCPU_PUSH(MinxCPU.BA.B.H);
			MinxCPU_PUSH(MinxCPU.BA.B.L);
			MinxCPU_PUSH(MinxCPU.HL.B.H);
			MinxCPU_PUSH(MinxCPU.HL.B.L);
			MinxCPU_PUSH(MinxCPU.X.B.H);
			MinxCPU_PUSH(MinxCPU.X.B.L);
			MinxCPU_PUSH(MinxCPU.Y.B.H);
			MinxCPU_PUSH(MinxCPU.Y.B.L);
			MinxCPU_PUSH(MinxCPU.N.B.H);
			return 48;
		case 0xB9: // PUSHAX
			MinxCPU_PUSH(MinxCPU.BA.B.H);
			MinxCPU_PUSH(MinxCPU.BA.B.L);
			MinxCPU_PUSH(MinxCPU.HL.B.H);
			MinxCPU_PUSH(MinxCPU.HL.B.L);
			MinxCPU_PUSH(MinxCPU.X.B.H);
			MinxCPU_PUSH(MinxCPU.X.B.L);
			MinxCPU_PUSH(MinxCPU.Y.B.H);
			MinxCPU_PUSH(MinxCPU.Y.B.L);
			MinxCPU_PUSH(MinxCPU.N.B.H);
			MinxCPU_PUSH(MinxCPU.HL.B.I);
			MinxCPU_PUSH(MinxCPU.X.B.I);
			MinxCPU_PUSH(MinxCPU.Y.B.I);
			return 60;

		case 0xBA: case 0xBB: // ??? #n
			return MinxCPU_ExecSPCF();

		case 0xBC: // POPA
			MinxCPU.N.B.H = MinxCPU_POP();
			MinxCPU.Y.B.L = MinxCPU_POP();
			MinxCPU.Y.B.H = MinxCPU_POP();
			MinxCPU.X.B.L = MinxCPU_POP();
			MinxCPU.X.B.H = MinxCPU_POP();
			MinxCPU.HL.B.L = MinxCPU_POP();
			MinxCPU.HL.B.H = MinxCPU_POP();
			MinxCPU.BA.B.L = MinxCPU_POP();
			MinxCPU.BA.B.H = MinxCPU_POP();
			return 44;

		case 0xBD: // POPAX
			MinxCPU.Y.B.I = MinxCPU_POP();
			MinxCPU.X.B.I = MinxCPU_POP();
			MinxCPU.HL.B.I = MinxCPU_POP();
			MinxCPU.N.B.I = MinxCPU.HL.B.I;
			MinxCPU.N.B.H = MinxCPU_POP();
			MinxCPU.Y.B.L = MinxCPU_POP();
			MinxCPU.Y.B.H = MinxCPU_POP();
			MinxCPU.X.B.L = MinxCPU_POP();
			MinxCPU.X.B.H = MinxCPU_POP();
			MinxCPU.HL.B.L = MinxCPU_POP();
			MinxCPU.HL.B.H = MinxCPU_POP();
			MinxCPU.BA.B.L = MinxCPU_POP();
			MinxCPU.BA.B.H = MinxCPU_POP();
			return 56;

		case 0xBE: case 0xBF: // ??? #n
			return MinxCPU_ExecSPCF();

		case 0xC0: // MOV BA, [HL]
			I16 = MinxCPU.HL.W.L;
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xC1: // MOV HL, [HL]
			I16 = MinxCPU.HL.W.L;
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xC2: // MOV X, [HL]
			I16 = MinxCPU.HL.W.L;
			MinxCPU.X.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.X.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;
		case 0xC3: // MOV Y, [HL]
			I16 = MinxCPU.HL.W.L;
			MinxCPU.Y.B.L = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16++);
			MinxCPU.Y.B.H = MinxCPU_OnRead(1, (MinxCPU.HL.B.I << 16) | I16);
			return 20;

		case 0xC4: // MOV [HL], BA
			I16 = MinxCPU.HL.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.BA.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 20;
		case 0xC5: // MOV [HL], HL
			I16 = MinxCPU.HL.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 20;
		case 0xC6: // MOV [HL], X
			I16 = MinxCPU.HL.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.X.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.X.B.H);
			return 20;
		case 0xC7: // MOV [HL], Y
			I16 = MinxCPU.HL.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16++, MinxCPU.Y.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.HL.B.I << 16) | I16, MinxCPU.Y.B.H);
			return 20;

		case 0xC8: case 0xC9: case 0xCA: case 0xCB: // MOV B, V
		case 0xCC: case 0xCD: case 0xCE: case 0xCF:
			MinxCPU.BA.B.H = MinxCPU.PC.B.I;
			return 12;

		case 0xD0: // MOV BA, [X]
			I16 = MinxCPU.X.W.L;
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16++);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 20;
		case 0xD1: // MOV HL, [X]
			I16 = MinxCPU.X.W.L;
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16++);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 20;
		case 0xD2: // MOV X, [X]
			I16 = MinxCPU.X.W.L;
			MinxCPU.X.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16++);
			MinxCPU.X.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 20;
		case 0xD3: // MOV Y, [X]
			I16 = MinxCPU.X.W.L;
			MinxCPU.Y.B.L = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16++);
			MinxCPU.Y.B.H = MinxCPU_OnRead(1, (MinxCPU.X.B.I << 16) | I16);
			return 20;

		case 0xD4: // MOV [X], BA
			I16 = MinxCPU.X.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16++, MinxCPU.BA.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 20;
		case 0xD5: // MOV [X], HL
			I16 = MinxCPU.X.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16++, MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 20;
		case 0xD6: // MOV [X], X
			I16 = MinxCPU.X.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16++, MinxCPU.X.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.X.B.H);
			return 20;
		case 0xD7: // MOV [X], Y
			I16 = MinxCPU.X.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16++, MinxCPU.Y.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.X.B.I << 16) | I16, MinxCPU.Y.B.H);
			return 20;

		case 0xD8: // MOV BA, [Y]
			I16 = MinxCPU.Y.W.L;
			MinxCPU.BA.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16++);
			MinxCPU.BA.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 20;
		case 0xD9: // MOV HL, [Y]
			I16 = MinxCPU.Y.W.L;
			MinxCPU.HL.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16++);
			MinxCPU.HL.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 20;
		case 0xDA: // MOV X, [Y]
			I16 = MinxCPU.Y.W.L;
			MinxCPU.X.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16++);
			MinxCPU.X.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 20;
		case 0xDB: // MOV Y, [Y]
			I16 = MinxCPU.Y.W.L;
			MinxCPU.Y.B.L = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16++);
			MinxCPU.Y.B.H = MinxCPU_OnRead(1, (MinxCPU.Y.B.I << 16) | I16);
			return 20;

		case 0xDC: // MOV [Y], BA
			I16 = MinxCPU.Y.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16++, MinxCPU.BA.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.BA.B.H);
			return 20;
		case 0xDD: // MOV [Y], HL
			I16 = MinxCPU.Y.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16++, MinxCPU.HL.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.HL.B.H);
			return 20;
		case 0xDE: // MOV [Y], X
			I16 = MinxCPU.Y.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16++, MinxCPU.X.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.X.B.H);
			return 20;
		case 0xDF: // MOV [Y], Y
			I16 = MinxCPU.Y.W.L;
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16++, MinxCPU.Y.B.L);
			MinxCPU_OnWrite(1, (MinxCPU.Y.B.I << 16) | I16, MinxCPU.Y.B.H);
			return 20;

		case 0xE0: // MOV BA, BA
			return 8;
		case 0xE1: // MOV BA, HL
			MinxCPU.BA.W.L = MinxCPU.HL.W.L;
			return 8;
		case 0xE2: // MOV BA, X
			MinxCPU.BA.W.L = MinxCPU.X.W.L;
			return 8;
		case 0xE3: // MOV BA, Y
			MinxCPU.BA.W.L = MinxCPU.Y.W.L;
			return 8;

		case 0xE4: // MOV HL, BA
			MinxCPU.HL.W.L = MinxCPU.BA.W.L;
			return 8;
		case 0xE5: // MOV HL, HL
			return 8;
		case 0xE6: // MOV HL, X
			MinxCPU.HL.W.L = MinxCPU.X.W.L;
			return 8;
		case 0xE7: // MOV HL, Y
			MinxCPU.HL.W.L = MinxCPU.Y.W.L;
			return 8;

		case 0xE8: // MOV X, BA
			MinxCPU.X.W.L = MinxCPU.BA.W.L;
			return 8;
		case 0xE9: // MOV X, HL
			MinxCPU.X.W.L = MinxCPU.HL.W.L;
			return 8;
		case 0xEA: // MOV X, X
			return 8;
		case 0xEB: // MOV X, Y
			MinxCPU.X.W.L = MinxCPU.Y.W.L;
			return 8;

		case 0xEC: // MOV Y, BA
			MinxCPU.Y.W.L = MinxCPU.BA.W.L;
			return 8;
		case 0xED: // MOV Y, HL
			MinxCPU.Y.W.L = MinxCPU.HL.W.L;
			return 8;
		case 0xEE: // MOV Y, X
			MinxCPU.Y.W.L = MinxCPU.X.W.L;
			return 8;
		case 0xEF: // MOV Y, Y
			return 8;

		case 0xF0: // MOV SP, BA
			MinxCPU.SP.W.L = MinxCPU.BA.W.L;
			return 8;
		case 0xF1: // MOV SP, HL
			MinxCPU.SP.W.L = MinxCPU.HL.W.L;
			return 8;
		case 0xF2: // MOV SP, X
			MinxCPU.SP.W.L = MinxCPU.X.W.L;
			return 8;
		case 0xF3: // MOV SP, Y
			MinxCPU.SP.W.L = MinxCPU.Y.W.L;
			return 8;

		case 0xF4: // MOV HL, SP
			MinxCPU.HL.W.L = MinxCPU.SP.W.L;
			return 8;
		case 0xF5: // MOV HL, PC
			MinxCPU.HL.W.L = MinxCPU.PC.W.L;
			return 8;
		case 0xF6: // ??? X
			MinxCPU.X.B.H = MinxCPU.PC.B.I;
			return 12;
		case 0xF7: // ??? Y
			MinxCPU.Y.B.H = MinxCPU.PC.B.I;
			return 12;

		case 0xF8: // MOV BA, SP
			MinxCPU.BA.W.L = MinxCPU.SP.W.L;
			return 8;
		case 0xF9: // MOV BA, PC
			MinxCPU.BA.W.L = MinxCPU.PC.W.L;
			return 8;

		case 0xFA: // MOV X, SP
			MinxCPU.X.W.L = MinxCPU.SP.W.L;
			return 8;

		case 0xFB: // NOTHING
		case 0xFC: // NOTHING
			return 12;
		case 0xFD: // MOV A, E
			MinxCPU.BA.B.L = MinxCPU.E;
			return 12;

		case 0xFE: // MOV Y, SP
			MinxCPU.Y.W.L = MinxCPU.SP.W.L;
			return 8;

		case 0xFF: // NOTHING
			return 64;

		default:
			MinxCPU_OnException(EXCEPTION_UNKNOWN_INSTRUCTION, (MinxCPU.IR << 8) + 0xCF);
			return 4;
	}
}

#pragma GCC pop_options
