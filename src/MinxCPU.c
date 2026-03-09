/*
  PokeMini - Pok�mon-Mini Emulator
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

#include <stdio.h>
#include <stdint.h>
#include <gba.h>
#include "include.h"

#define GFX_SZ_TOLERANCE	1024

TMinxCPU MinxCPU;
//extern TMinxPRC MinxPRC;
EWRAM_DATA uint8_t minx_ram[4096];

//
// Functions
//

int MinxCPU_Create(void)
{
	// Init variables
	MinxCPU.BA.D = 0;
	MinxCPU.HL.D = 0;
	MinxCPU.X.D = 0;
	MinxCPU.Y.D = 0;
	MinxCPU.SP.D = 0;
	MinxCPU.PC.D = 0;
	MinxCPU.N.D = 0;
	MinxCPU.E = 0;
	MinxCPU.F = 0xC0;
	Set_U(0);
	MinxCPU.Status = MINX_STATUS_NORMAL;
	return 1;
}

void MinxCPU_Destroy(void)
{
	// Nothing...
}

// Reset core, call it after OnRead/OnWrite point to the right BIOS
void MinxCPU_Reset(int hardreset)
{
	MinxCPU.Status = MINX_STATUS_NORMAL;
	MinxCPU.PC.W.L = ReadMem16(hardreset ? 0 : 2);
	MinxCPU.E = 0x1F;
	MinxCPU.F = 0xC0;
	Set_U(0);
	MinxCPU_OnIRQHandle(MinxCPU.F, MinxCPU.Shift_U);
}

// Load State
int MinxCPU_LoadState(FILE *fi, uint32_t bsize)
{
    //TODO
    return 0;
}

// Save State
int MinxCPU_SaveState(FILE *fi)
{
    //TODO
    return 0;
}

// Force call Interrupt by the address
int MinxCPU_CallIRQ(uint8_t addr)
{
	if (MinxCPU.Status == MINX_STATUS_IRQ) return 0;
	MinxCPU.Status = MINX_STATUS_IRQ;
	MinxCPU.IRQ_Vector = addr;
	return 1;
}

IWRAM_CODE ARM_CODE
uint8_t MinxCPU_OnRead(int cpu, uint32_t addr)
{
    if (addr >= 0x000000 && addr <= 0x000FFF) //BIOS
		return bios_bin[addr&0x0FFF];
	else
	if (addr >= 0x001000 && addr <= 0x001FFF) //RAM
		return minx_ram[(addr-0x1000)&0x0FFF];
	else
	if (addr >= 0x002000 && addr <= 0x0020FF) //Registers
		return minx_read_reg(addr&0x0000FF);
	else
	if (addr >= 0x002100 && addr <= 0x1FFFFF) //Cart ROM
		return rom_bin[(addr/*-0x2100*/)&0x1FFFFF];
	else
	if ((addr>>20) >= 2) //Cart ROM (mirror)
		return rom_bin[addr&0x1FFFFF];
	else
		return 0x00;
}

IWRAM_CODE ARM_CODE
void MinxCPU_OnWrite(int cpu, uint32_t addr, uint8_t data)
{
	if (addr >= 0x000000 && addr <= 0x000FFF) //BIOS
		return;
	else
	if (addr >= 0x001000 && addr <= 0x001FFF) //RAM
	{
		uint32_t addr_rel= (addr-0x1000)&0x0FFF;
		minx_ram[addr_rel]= data;

		if (addr <= 0x0012FF) //Frame buffer write
		{
			if ((MinxRegs[VREG_PRC_MODE]&PRC_MODE_ENA_MAP))
				return;

			host_vram_write(addr_rel, data);
			return;
		}
		else if (addr <= 0x00135F) //OAM write
		{
			if (!(MinxRegs[VREG_PRC_MODE]&PRC_MODE_ENA_SPR))
				return;

			prc_on_oam_update((addr_rel-0x300)>>2);
			return;
		}
		else if (addr <= 0x001360+lut_prc_map_bytes[MinxRegs[VREG_PRC_MODE]>>4]) //Map screen write
		{
			if (!(MinxRegs[VREG_PRC_MODE]&PRC_MODE_ENA_MAP))
				return;

			uint16_t slot= (addr_rel-0x360);
			//uint16_t map_rows= lut_maph[MinxRegs[VREG_PRC_MODE]>>4];
			uint16_t map_cols= lut_mapw[MinxRegs[VREG_PRC_MODE]>>4];
			uint16_t tile_row= (slot/map_cols);
			uint16_t tile_col= (slot%map_cols);
			uint16_t* vram_addr= &GFX_MAP_SCR_ADR[(tile_row+tile_col*32)>>1];
			if (tile_row&1)
				*vram_addr= (data<<8)|(*vram_addr&0x00FF);
			else
				*vram_addr= (*vram_addr&0xFF00)|data;

			return;
		}

		uint32_t prc_spr_base= (MinxRegs[VREG_PRC_SPR_HI]<<16)|
								(MinxRegs[VREG_PRC_SPR_MID]<<8)|
								MinxRegs[VREG_PRC_SPR_LO];

		if (addr >= prc_spr_base && addr <= prc_spr_base+GFX_SZ_TOLERANCE)
			//Likely using RAM as a sprite tile base, load the data in VRAM
			prc_pending_updates |= PRC_QUEUE_COPY_SPR_GFX | PRC_QUEUE_FORCE_UPDATE | PRC_QUEUE_WAIT;
	}
	else
	if (addr >= 0x002000 && addr <= 0x002FFF) //Registers
		minx_set_reg(addr&0x0000FF, data);
	else
	if (addr >= 0x002100 && addr <= 0x1FFFFF) //Cart ROM
		return;
	else
	if ((addr>>20) >= 2) //Cart ROM (mirror)
		return;
}

void MinxCPU_OnException(int type, uint32_t opc)
{
	consoleDemoInit();

	irqEnable(IRQ_VBLANK);
	irqSet(IRQ_VBLANK, NULL);

	iprintf(":(\nYour PM ran into a problem and needs to restart.\nHere's what has gone wrong:\n\n");

	switch (type)
	{
		case EXCEPTION_UNKNOWN_INSTRUCTION:
			iprintf("UNKNOWN INSTRUCTION");
			break;
		case EXCEPTION_CRASH_INSTRUCTION:
			iprintf("CRASH INSTRUCTION");
			break;
		case EXCEPTION_UNSTABLE_INSTRUCTION:
			iprintf("UNSTABLE INSTRUCTION");
			break;
		case EXCEPTION_DIVISION_BY_ZERO:
			iprintf("DIVISION BY ZERO");
			break;
	}

	iprintf("\nOP:0x%lx\nPC:0x%x", opc, MinxCPU.PC.W.L|(MinxCPU.PC.W.H<<16));

	while(1)
	{
		VBlankIntrWait();
	}
}
void MinxCPU_OnSleep(int type)
{
	//Enter sleep mode
	irqDisable(IRQ_VBLANK);
	irqDisable(IRQ_VCOUNT);

	uint16_t kh= 0;
	do
	{
		scanKeys();
		kh= keysHeld();
	}
	while (kh&KEY_START);

	irqEnable(IRQ_KEYPAD);
	REG_KEYCNT= KEY_START | KEYIRQ_AND | KEYIRQ_ENABLE;
	REG_DISPCNT= (REG_DISPCNT&0xFF7F)|LCDC_OFF;
	Stop();

	//Exit sleep mode
	//REG_IF= IRQ_KEYPAD;
	if (MinxRegs[VREG_PRC_MODE]&PRC_MODE_ENA_COPY)
		irqEnable(IRQ_VCOUNT);
	else
		irqDisable(IRQ_VCOUNT);
	irqEnable(IRQ_VBLANK);
	REG_DISPCNT= (REG_DISPCNT&0xFF7F);
	irqDisable(IRQ_KEYPAD);

	//MinxCPU.Status = MINX_STATUS_NORMAL;
	SoftReset(0);
}
void MinxCPU_OnIRQHandle(uint8_t flag, uint8_t shift_u)
{
	//BG_PALETTE[0]= 0x0000;
	// ((uint8_t*)EWRAM)[0x1010]= 'I';
	// ((uint8_t*)EWRAM)[0x1011]= 'R';
	// ((uint8_t*)EWRAM)[0x1012]= 'Q';
	// ((uint8_t*)EWRAM)[0x1013]= flag;
	//VBlankIntrWait();
	//BG_PALETTE[0]= RGB5(24,28,24);
	// consoleDemoInit();
 //
	// irqEnable(IRQ_VBLANK);
	// irqSet(IRQ_VBLANK, NULL);
 //
	// iprintf("IRQ");
 //
	// while(1)
	// {
	// 	VBlankIntrWait();
	// }
}
void MinxCPU_OnIRQAct(uint8_t intr)
{
	consoleDemoInit();

	irqEnable(IRQ_VBLANK);
	irqSet(IRQ_VBLANK, NULL);

	iprintf("Int ACT");

	while(1)
	{
		VBlankIntrWait();
	}
}
