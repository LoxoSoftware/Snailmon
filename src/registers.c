#include <gba.h>
#include "include.h"

//uint8_t MinxRegs[256];
uint8_t* MinxRegs= (u8*)(EWRAM+0x1100);

extern uint8_t irq_act1_pending;
extern uint8_t irq_act2_pending;
extern uint8_t irq_act3_pending;
extern uint8_t irq_act4_pending;

void minx_set_reg(int reg, uint8_t data)
{
    switch(reg)
    {
        case VREG_PRC_MODE:
            MinxRegs[reg]= data;
            switch ((data>>1)&0x07)
            {
                case 0b000: case 0b010: case 0b001: case 0b011:
                    //PRC disabled (no frame copy)
                    irqDisable(IRQ_VCOUNT);
                    //REG_DISPCNT= (REG_DISPCNT&0xFF7F)|LCDC_OFF;
                    REG_DISPCNT= (REG_DISPCNT&0xEB7F);
                    break;
                case 0b101: case 0b111:
                    //Map rendering enabled
                    irqEnable(IRQ_VCOUNT);
                    REG_DISPCNT= (REG_DISPCNT&0xEB7F)|BG2_ON|(data&0x4?OBJ_ON:0);
                    break;
                case 0b100: case 0b110:
                    //Map rendering disabled
                    irqEnable(IRQ_VCOUNT);
                    REG_DISPCNT= (REG_DISPCNT&0xEB7F)|BG2_ON|(data&0x4?OBJ_ON:0);
                    prc_on_fcopy_mode();
                    break;
            }
            return;
        case VREG_PRC_MAP_LO: case VREG_PRC_MAP_MID: case VREG_PRC_MAP_HI:
            MinxRegs[reg]= data;
            //prc_on_map_addr_change();
            prc_pending_updates |= PRC_QUEUE_COPY_BG_GFX;
            return;
        case VREG_PRC_SPR_LO: case VREG_PRC_SPR_MID: case VREG_PRC_SPR_HI:
            MinxRegs[reg]= data;
            //prc_on_spr_addr_change();
            prc_pending_updates |= PRC_QUEUE_COPY_SPR_GFX;
            return;
        case VREG_IRQ_ACT1:
            MinxRegs[reg]= 0x00;
            irq_act1_pending |= data;
            return;
        case VREG_IRQ_ACT2:
            MinxRegs[reg]= 0x00;
            irq_act2_pending |= data;
            return;
        case VREG_IRQ_ACT3:
            MinxRegs[reg]= 0x00;
            irq_act3_pending |= data;
            return;
        case VREG_IRQ_ACT4:
            MinxRegs[reg]= 0x00;
            irq_act4_pending |= data;
            return;
        default:
            MinxRegs[reg]= data;
            return;
    }
}

uint8_t minx_read_reg(int reg)
{
    // switch(reg)
    // {
    //     default:
    //         return 0x00;
    // }
    return MinxRegs[reg];
}
