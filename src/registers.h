/*
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

#ifndef REGISTERS_H
#define REGISTERS_H

#define PRC_MODE_INV_MAP    (1)
#define PRC_MODE_ENA_MAP    (1<<1)
#define PRC_MODE_ENA_SPR    (1<<2)
#define PRC_MODE_ENA_COPY   (1<<3)
#define PRC_MODE_MAP_SZ     (1<<4) //2 bits

enum
{
    VREG_SYS_CTRL1=     0x00,
    VREG_SYS_CTRL2=     0x01,
    VREG_SYS_CTRL3=     0x02,
    VREG_SEC_CTRL=      0x08,
    VREG_SEC_CNT_LO=    0x09,
    VREG_SEC_CNT_MID=   0x0A,
    VREG_SEC_CNT_HI=    0x0B,
    VREG_TMR1_SCALE=    0x18,
    VREG_TMR1_OSC=      0x19,
    VREG_TMR2_SCALE=    0x1A,
    VREG_TMR2_OSC=      0x1B,
    VREG_TMR3_SCALE=    0x1C,
    VREG_TMR3_OSC=      0x1D,
    VREG_IRQ_PRI1=      0x20,
    VREG_IRQ_PRI2=      0x21,
    VREG_IRQ_PRI3=      0x22,
    VREG_IRQ_ENA1=      0x23,
    VREG_IRQ_ENA2=      0x24,
    VREG_IRQ_ENA3=      0x25,
    VREG_IRQ_ENA4=      0x26,
    VREG_IRQ_ACT1=      0x27,
    VREG_IRQ_ACT2=      0x28,
    VREG_IRQ_ACT3=      0x29,
    VREG_IRQ_ACT4=      0x2A,
    VREG_TMR1_CTRL_L=   0x30,
    VREG_TMR1_CTRL_H=   0x31,
    VREG_TMR1_PRE_L=    0x32,
    VREG_TMR1_PRE_H=    0x33,
    VREG_TMR1_PVT_L=    0x34,
    VREG_TMR1_PVT_H=    0x35,
    VREG_TMR1_CNT_L=    0x36,
    VREG_TMR1_CNT_H=    0x37,
    VREG_TMR2_CTRL_L=   0x38,
    VREG_TMR2_CTRL_H=   0x39,
    VREG_TMR2_PRE_L=    0x3A,
    VREG_TMR2_PRE_H=    0x3B,
    VREG_TMR2_PVT_L=    0x3C,
    VREG_TMR2_PVT_H=    0x3D,
    VREG_TMR2_CNT_L=    0x3E,
    VREG_TMR2_CNT_H=    0x3F,
    VREG_TMR3_CTRL_L=   0x48,
    VREG_TMR3_CTRL_H=   0x49,
    VREG_TMR3_PRE_L=    0x4A,
    VREG_TMR3_PRE_H=    0x4B,
    VREG_TMR3_PVT_L=    0x4C,
    VREG_TMR3_PVT_H=    0x4D,
    VREG_TMR3_CNT_L=    0x4E,
    VREG_TMR3_CNT_H=    0x4F,
    VREG_KEYPAD=        0x52,
    VREG_IO_DIR=        0x60,
    VREG_IO_DATA=       0x61,
    VREG_PRC_MODE=      0x80,
    VREG_PRC_MAP_LO=    0x82,
    VREG_PRC_MAP_MID=   0x83,
    VREG_PRC_MAP_HI=    0x84,
    VREG_PRC_SCROLL_Y=  0x85,
    VREG_PRC_SCROLL_X=  0x86,
    VREG_PRC_SPR_LO=    0x87,
    VREG_PRC_SPR_MID=   0x88,
    VREG_PRC_SPR_HI=    0x89,
    VREG_PRC_CNT=       0x8A,
    VREG_LCD_CTRL=      0xFE,
    //Fake registers
    VREG_PRC_PALETTE=   0x8B,
    VREG_PRC_MODE_OLD=  0x90,
    VREG_IO_DATA_OLD=   0x91,
};

//extern uint8_t MinxRegs[256];
extern uint8_t* MinxRegs;

void minx_set_reg(int reg, uint8_t data);

uint8_t minx_read_reg(int reg);

void reg_sec_counter_increment();

#endif
