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
    VREG_KEYPAD=        0x52,
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
};

//extern uint8_t MinxRegs[256];
extern uint8_t* MinxRegs;

void minx_set_reg(int reg, uint8_t data);

uint8_t minx_read_reg(int reg);

#endif
