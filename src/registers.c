#include <gba.h>
#include "include.h"

//uint8_t MinxRegs[256];
uint8_t* MinxRegs= (u8*)(EWRAM+0x1100);

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
                    break;
                case 0b101: case 0b111:
                    //Map rendering enabled
                    irqEnable(IRQ_VCOUNT);
                    break;
                case 0b100: case 0b110:
                    //Map rendering disabled
                    irqEnable(IRQ_VCOUNT);
                    break;
            }
            return;
        case VREG_PRC_MAP_HI: case VREG_PRC_MAP_MID: case VREG_PRC_MAP_LO:
            MinxRegs[reg]= data;
            prc_on_map_addr_change();
            return;
        case VREG_PRC_SPR_HI: case VREG_PRC_SPR_MID: case VREG_PRC_SPR_LO:
            MinxRegs[reg]= data;
            prc_on_spr_addr_change();
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
