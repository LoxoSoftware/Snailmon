/*
  PokeMini - Pokémon-Mini Emulator
  Copyright (C) 2009-2012  JustBurn

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

#ifndef MINXCPU_CORE
#define MINXCPU_CORE

#include <stdio.h>
#include <stdint.h>

//#define _BIG_ENDIAN

#ifdef _BIG_ENDIAN

typedef union {
	struct {
		uint8_t X;
		uint8_t I;
		uint8_t H;
		uint8_t L;
	} B;
	struct {
		uint16_t H;
		uint16_t L;
	} W;
	uint32_t D;
} MinxRegx;

#else

typedef union {
	struct {
		uint8_t L;
		uint8_t H;
		uint8_t I;
		uint8_t X;
	} B;
	struct {
		uint16_t L;
		uint16_t H;
	} W;
	uint32_t D;
} MinxRegx;

#endif

#define MINX_FLAG_ZERO    	0x01
#define MINX_FLAG_CARRY   	0x02
#define MINX_FLAG_OVERFLOW	0x04
#define MINX_FLAG_SIGN    	0x08
#define MINX_FLAG_BCD     	0x10
#define MINX_FLAG_NIBBLE  	0x20
#define MINX_FLAG_INTFLG  	0x40
#define MINX_FLAG_INTOFF  	0x80

#define MINX_FLAG_SAVE_NUL	0xF0
#define MINX_FLAG_SAVE_O  	0xF4
#define MINX_FLAG_SAVE_CO 	0xF6
#define MINX_FLAG_SAVE_COS	0xFE

// OnException() reasons
enum {
	EXCEPTION_UNKNOWN_INSTRUCTION,
	EXCEPTION_CRASH_INSTRUCTION,
	EXCEPTION_UNSTABLE_INSTRUCTION,
	EXCEPTION_DIVISION_BY_ZERO
};

// OnSleep() reasons
enum {
	MINX_SLEEP_MinxCPU_HALT,
	MINX_SLEEP_MinxCPU_STOP
};

// Status reasons
enum {
	MINX_STATUS_NORMAL = 0, // Normal operation
	MINX_STATUS_MinxCPU_HALT = 1,   // CPU during MinxCPU_HALT
	MINX_STATUS_MinxCPU_STOP = 2,   // CPU during MinxCPU_STOP
	MINX_STATUS_IRQ = 3,    // Delay caused by hardware IRQ
};

// DebugHalt reasons
enum {
	DEBUG_HALT_RECEIVE,
	DEBUG_HALT_SUSPEND,
	DEBUG_HALT_RESUME,
};

#ifndef inline
#define inline __inline
#endif

// Signed 8-Bits to 16-Bits converter
uint16_t S8_TO_16(int8_t a);

typedef struct {
	// Registers
	MinxRegx BA;			// Registers A, B
	MinxRegx HL;			// Registers L, H, I
	MinxRegx X;			// Registers X, XI
	MinxRegx Y;			// Registers Y, YI
	MinxRegx SP;			// Register SP
	MinxRegx PC;			// Registers PC, V
	MinxRegx N;			// for [N+#nn], I is written here too
	uint8_t U1;			// V Shadow 1
	uint8_t U2;			// V Shadow 2
	uint8_t F;			// Flags
	uint8_t E;			// Exception
	uint8_t IR;			// Last Instruction Register (for open-bus)
	uint8_t Shift_U;		// Shift U, set to 2 when: U modify, branch, return
	uint8_t Status;			// CPU Status (0 = Normal, 1 = Halt, 2 = Stoped, 3 = IRQ)
	uint8_t IRQ_Vector;		// IRQ Vector when Status is IRQ
	uint8_t Reserved[28];		// Reserved bytes
} TMinxCPU;

// CPU registers
extern TMinxCPU MinxCPU;

// Callbacks (Must be coded by the user)
uint8_t MinxCPU_OnRead(int cpu, uint32_t addr);
void MinxCPU_OnWrite(int cpu, uint32_t addr, uint8_t data);
void MinxCPU_OnException(int type, uint32_t opc);
void MinxCPU_OnSleep(int type);
void MinxCPU_OnIRQHandle(uint8_t flag, uint8_t shift_u);
void MinxCPU_OnIRQAct(uint8_t intr);

// Functions
int MinxCPU_Create(void);		// Create MinxCPU
void MinxCPU_Destroy(void);		// Destroy MinxCPU
void MinxCPU_Reset(int hardreset);	// Reset CPU
int MinxCPU_LoadState(FILE *fi, uint32_t bsize); // Load State
int MinxCPU_SaveState(FILE *fi);	// Save State
int MinxCPU_Exec(void);			// Execute 1 CPU instruction
int MinxCPU_CallIRQ(uint8_t IRQ);	// Call an IRQ

// Helpers
uint16_t ReadMem16(uint32_t addr);
void WriteMem16(uint32_t addr, uint16_t data);
uint8_t Fetch8(void);
uint16_t Fetch16(void);
void Set_U(uint8_t val);

// Instruction exec. prototypes

int MinxCPU_ExecCE(void);
int MinxCPU_ExecCF(void);
int MinxCPU_ExecSPCE(void);
int MinxCPU_ExecSPCF(void);

#endif
