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

#ifndef INTERRUPT_H
#define INTERRUPT_H

#define VIRQ_PRC_COPY_DONE      3
#define VIRQ_PRC_FRAMEDIV_OF    4
#define VIRQ_TMR2_UPPER_UF      5
#define VIRQ_TMR2_UPPER_UF_8BIT 6
#define VIRQ_TMR256_2HZ         13
#define VIRQ_CART_EJECT         19
#define VIRQ_INPUT_KEY_A        28
#define VIRQ_INPUT_KEY_B        27
#define VIRQ_INPUT_KEY_C        26
#define VIRQ_INPUT_KEY_UP       25
#define VIRQ_INPUT_KEY_DOWN     24
#define VIRQ_INPUT_KEY_LEFT     23
#define VIRQ_INPUT_KEY_RIGHT    22
#define VIRQ_INPUT_KEY_POWER    21
#define VIRQ_INPUT_SHOCK        16

uint8_t irq_act1_pending= 0x00;
uint8_t irq_act2_pending= 0x00;
uint8_t irq_act3_pending= 0x00;
uint8_t irq_act4_pending= 0x00;

inline void irq_act(uint8_t act, uint8_t mask)
{
    switch (act)
    {
        case 1:
            if (!(irq_act1_pending&mask))
                return;
            MinxRegs[VREG_IRQ_ACT1] |= mask;
            irq_act1_pending &= ~mask;
            break;
        case 2:
            if (!(irq_act2_pending&mask))
                return;
            MinxRegs[VREG_IRQ_ACT2] |= mask;
            irq_act2_pending &= ~mask;
            break;
        case 3:
            if (!(irq_act3_pending&mask))
                return;
            MinxRegs[VREG_IRQ_ACT3] |= mask;
            irq_act3_pending &= ~mask;
            break;
        case 4:
            if (!(irq_act4_pending&mask))
                return;
            MinxRegs[VREG_IRQ_ACT4] |= mask;
            irq_act4_pending &= ~mask;
            break;
        default:
            return;
    }
}

void send_irq(int irq)
{
    switch (irq)
    {
        case VIRQ_PRC_COPY_DONE:
            if (MinxRegs[VREG_IRQ_ENA1]&0x80 && MinxRegs[VREG_IRQ_PRI1]&0xC0)
                MinxCPU_CallIRQ(VIRQ_PRC_COPY_DONE<<1);
            else
                irq_act(1, 0x80);
        return;
        case VIRQ_PRC_FRAMEDIV_OF:
            if (MinxRegs[VREG_IRQ_ENA1]&0x40 && MinxRegs[VREG_IRQ_PRI1]&0xC0)
                MinxCPU_CallIRQ(VIRQ_PRC_FRAMEDIV_OF<<1);
            else
                irq_act(1, 0x40);
        return;
        case VIRQ_TMR2_UPPER_UF:
            if (MinxRegs[VREG_IRQ_ENA1]&0x20 && MinxRegs[VREG_IRQ_PRI1]&0x30)
                MinxCPU_CallIRQ(VIRQ_TMR2_UPPER_UF<<1);
            else
                irq_act(1, 0x20);
        return;
        case VIRQ_TMR256_2HZ:
            if (MinxRegs[VREG_IRQ_ENA2]&0x08 && MinxRegs[VREG_IRQ_PRI2]&0xC0)
                MinxCPU_CallIRQ(VIRQ_TMR256_2HZ<<1);
            else
                irq_act(2, 0x08);
        return;
        case VIRQ_CART_EJECT:
            if (MinxRegs[VREG_IRQ_ENA2]&0x02 && MinxRegs[VREG_IRQ_PRI2]&0x30)
                MinxCPU_CallIRQ(VIRQ_CART_EJECT<<1);
            else
                irq_act(2, 0x02);
        return;
        case VIRQ_INPUT_KEY_A:
            if (MinxRegs[VREG_IRQ_ENA3]&0x01 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_A<<1);
            else
                irq_act(3, 0x01);
        return;
        case VIRQ_INPUT_KEY_B:
            if (MinxRegs[VREG_IRQ_ENA3]&0x02 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_B<<1);
            else
                irq_act(3, 0x02);
        return;
        case VIRQ_INPUT_KEY_C:
            if (MinxRegs[VREG_IRQ_ENA3]&0x04 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_C<<1);
            else
                irq_act(3, 0x04);
        return;
        case VIRQ_INPUT_KEY_UP:
            if (MinxRegs[VREG_IRQ_ENA3]&0x08 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_UP<<1);
            else
                irq_act(3, 0x08);
        return;
        case VIRQ_INPUT_KEY_DOWN:
            if (MinxRegs[VREG_IRQ_ENA3]&0x10 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_DOWN<<1);
            else
                irq_act(3, 0x10);
        return;
        case VIRQ_INPUT_KEY_LEFT:
            if (MinxRegs[VREG_IRQ_ENA3]&0x20 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_LEFT<<1);
            else
                irq_act(3, 0x20);
        return;
        case VIRQ_INPUT_KEY_RIGHT:
            if (MinxRegs[VREG_IRQ_ENA3]&0x40 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_RIGHT<<1);
            else
                irq_act(3, 0x40);
        return;
        case VIRQ_INPUT_KEY_POWER:
            if (MinxRegs[VREG_IRQ_ENA3]&0x80 && MinxRegs[VREG_IRQ_PRI2]&0x0C)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_POWER<<1);
            else
                irq_act(3, 0x80);
        return;
        case VIRQ_INPUT_SHOCK:
            if (MinxRegs[VREG_IRQ_ENA4]&0x40 && MinxRegs[VREG_IRQ_PRI3]&0x03)
                MinxCPU_CallIRQ(VIRQ_INPUT_SHOCK<<1);
            else
                irq_act(4, 0x40);
        return;
        default:
            break;
    }
}

#endif
