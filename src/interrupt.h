#ifndef INTERRUPT_H
#define INTERRUPT_H

#define VIRQ_PRC_COPY_DONE      3
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

void send_irq(int irq)
{
    switch (irq)
    {
        case VIRQ_PRC_COPY_DONE:
            if (MinxRegs[VREG_IRQ_ENA1]&0x80)
                MinxCPU_CallIRQ(VIRQ_PRC_COPY_DONE<<1);
        return;
        case VIRQ_TMR2_UPPER_UF:
            if (MinxRegs[VREG_IRQ_ENA1]&0x20)
                MinxCPU_CallIRQ(VIRQ_TMR2_UPPER_UF<<1);
        return;
        case VIRQ_TMR256_2HZ:
            if (MinxRegs[VREG_IRQ_ENA2]&0x08)
                MinxCPU_CallIRQ(VIRQ_TMR256_2HZ<<1);
        return;
        case VIRQ_CART_EJECT:
            if (MinxRegs[VREG_IRQ_ENA2]&0x02)
                MinxCPU_CallIRQ(VIRQ_CART_EJECT<<1);
        return;
        case VIRQ_INPUT_KEY_A:
            if (MinxRegs[VREG_IRQ_ENA3]&0x01)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_A<<1);
        return;
        case VIRQ_INPUT_KEY_B:
            if (MinxRegs[VREG_IRQ_ENA3]&0x02)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_B<<1);
        return;
        case VIRQ_INPUT_KEY_C:
            if (MinxRegs[VREG_IRQ_ENA3]&0x04)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_C<<1);
        return;
        case VIRQ_INPUT_KEY_UP:
            if (MinxRegs[VREG_IRQ_ENA3]&0x08)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_UP<<1);
        return;
        case VIRQ_INPUT_KEY_DOWN:
            if (MinxRegs[VREG_IRQ_ENA3]&0x10)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_DOWN<<1);
        return;
        case VIRQ_INPUT_KEY_LEFT:
            if (MinxRegs[VREG_IRQ_ENA3]&0x20)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_LEFT<<1);
        return;
        case VIRQ_INPUT_KEY_RIGHT:
            if (MinxRegs[VREG_IRQ_ENA3]&0x40)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_RIGHT<<1);
        return;
        case VIRQ_INPUT_KEY_POWER:
            if (MinxRegs[VREG_IRQ_ENA3]&0x80)
                MinxCPU_CallIRQ(VIRQ_INPUT_KEY_POWER<<1);
        return;
        case VIRQ_INPUT_SHOCK:
            if (MinxRegs[VREG_IRQ_ENA4]&0x40)
                MinxCPU_CallIRQ(VIRQ_INPUT_SHOCK<<1);
        return;
        default:
            break;
    }
}

#endif
