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

#include <gba.h>
#include <string.h>
#include "include.h"
#include "interrupt.h"

#define vSCREEN_XOFS    24
#define vSCREEN_YOFS    16

extern TMinxCPU MinxCPU;
extern u8 minx_ram[];

uint32_t frames= 0;

const u16* bios_irq_vect= (u16*)bios_bin;

bool option_mask_screen= true;
bool option_72hz_refresh= false;

const uint8_t PM_IO_INIT[256] = {
	0x7F, 0x20, 0x5C, 0xff, 0xff, 0xff, 0xff, 0xff, // $00~$07 System Control
	0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, // $08~$0F Second Counter
	0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $10~$17 Battery Sensor
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, // $18~$1F Timers Controls
	0x00, 0x30, 0x02, 0x00, 0x02, 0x00, 0x00, 0x40, // $20~$27 IRQ
	0x00, 0xC0, 0x40, 0xff, 0xff, 0xff, 0xff, 0xff, // $28~$2F IRQ
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, // $30~$37 Timer 1
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, // $38~$3F Timer 2
	0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, // $40~$47 256Hz Counter + ???
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, // $48~$4F Timer 3
	0xFF, 0x00, 0xFF, 0x00, 0x01, 0x01, 0xff, 0xff, // $50~$57 Keypad + ???
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $58~$5F Unused
	0x32, 0x64, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, // $60~$67 I/O
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $68~$6F Unused
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $70~$77 Audio + ???
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $78~$7F Unused
	0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // $80~$87 PRC
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // $88~$8F PRC
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $90~$97 Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $98~$9F Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $A0~$A7 Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $A8~$AF Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $B0~$B7 Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $B8~$BF Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $C0~$C7 Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $C8~$CF Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $D0~$D7 Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $D8~$DF Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $E0~$E7 Unused
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // $E8~$EF Unused
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // $F0~$F7 ???
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0xFF  // $F8~$FF LCD I/O
};

IWRAM_CODE ARM_CODE
void isr_prc_copy_complete()
{
    //if (MinxRegs[VREG_PRC_MODE]&0x04) //Only if PRC copy is enabled
    if (!(frames&((MinxRegs[VREG_PRC_MODE]&PRC_MODE_ENA_MAP)?1:3))) //Slow down the rate if software rendering
        send_irq(VIRQ_PRC_COPY_DONE);                               // is detected (the GBA can't do it fast enough)
    frames++;
}

IWRAM_CODE ARM_CODE
void isr_display()
{
    irqDisable(IRQ_VBLANK);

    REG_BG2X= ((-8+(MinxRegs[VREG_PRC_SCROLL_Y]&0x7F))<<8);
    REG_BG2Y= (-12+(MinxRegs[VREG_PRC_SCROLL_X]&0x7F))<<8;

    //Set keypad register
    //  MSB Pw|Ri|Le|Do|Up|C|B|A LSB
    MinxRegs[VREG_KEYPAD]=
        /* A -> A */ ((REG_KEYINPUT) & 0b00000001) |
        /* B -> B */ ((REG_KEYINPUT) & 0b00000010) |
        /* R -> C */ ((REG_KEYINPUT>>6) & 0b00000100) |
        /* Up-> Up*/ ((REG_KEYINPUT>>3) & 0b00001000) |
        /* Do-> Do*/ ((REG_KEYINPUT>>3) & 0b00010000) |
        /* Le-> Le*/ ((REG_KEYINPUT) & 0b00100000) |
        /* Ri-> Ri*/ ((REG_KEYINPUT<<2) & 0b01000000) |
        /* St-> Pw*/ ((REG_KEYINPUT<<4) & 0b10000000);

    ((u8*)EWRAM)[0x1000]= MinxCPU.PC.B.L;
    ((u8*)EWRAM)[0x1001]= MinxCPU.PC.B.H;
    ((u8*)EWRAM)[0x1002]= MinxCPU.PC.B.I;
    ((u8*)EWRAM)[0x1003]= MinxCPU.PC.B.X;
    ((u8*)EWRAM)[0x1004]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L));
    ((u8*)EWRAM)[0x1005]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L)+1);
    ((u8*)EWRAM)[0x1006]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L)+2);
    ((u8*)EWRAM)[0x1007]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L)+3);
    ((u8*)EWRAM)[0x1008]= MinxCPU.BA.B.L;
    ((u8*)EWRAM)[0x1009]= MinxCPU.BA.B.H;
    ((u8*)EWRAM)[0x100A]= MinxCPU.BA.B.I;
    ((u8*)EWRAM)[0x100B]= MinxCPU.BA.B.X;
    ((u8*)EWRAM)[0x100C]= MinxCPU.SP.B.L;
    ((u8*)EWRAM)[0x100D]= MinxCPU.SP.B.H;
    ((u8*)EWRAM)[0x100E]= MinxCPU.SP.B.I;
    ((u8*)EWRAM)[0x100F]= MinxCPU.SP.B.X;
    //*((u32*)0x2001010)= (u32)rom_bin;

    scanKeys();
    u16 kd= keysDown();

    //Toggle screen mask
    if (kd&KEY_SELECT)
    {
        option_mask_screen= !option_mask_screen;
        SetMode(MODE_1|BG2_ON|OBJ_ON|OBJ_1D_MAP|(option_mask_screen?WIN0_ON:0));
        prc_pending_updates |= PRC_QUEUE_COPY_BG_GFX | PRC_QUEUE_COPY_SPR_GFX | PRC_QUEUE_FORCE_UPDATE;
    }
    if (kd&KEY_A)
        send_irq(VIRQ_INPUT_KEY_A);
    if (kd&KEY_B)
        send_irq(VIRQ_INPUT_KEY_B);
    if (kd&KEY_R)
        send_irq(VIRQ_INPUT_KEY_C);
    if (kd&KEY_UP)
        send_irq(VIRQ_INPUT_KEY_UP);
    if (kd&KEY_DOWN)
        send_irq(VIRQ_INPUT_KEY_DOWN);
    if (kd&KEY_LEFT)
        send_irq(VIRQ_INPUT_KEY_LEFT);
    if (kd&KEY_RIGHT)
        send_irq(VIRQ_INPUT_KEY_RIGHT);
    if (kd&KEY_START)
        send_irq(VIRQ_INPUT_KEY_POWER);
    if (kd&KEY_L)
        //send_irq(VIRQ_TMR2_UPPER_UF);
        send_irq(VIRQ_INPUT_SHOCK);

    if (prc_pending_updates&PRC_QUEUE_WAIT)
    {
        //IF PRQ_QUEUE_WAIT is set, we shall wait until the flag stops bein set
        prc_pending_updates &= (~PRC_QUEUE_WAIT); //Unset the flag
    }
    else
    {
        if (prc_pending_updates&PRC_QUEUE_COPY_BG_GFX)
            prc_on_map_addr_change();
        if (prc_pending_updates&PRC_QUEUE_COPY_SPR_GFX)
            prc_on_spr_addr_change();
    }

    if (!option_72hz_refresh)
        isr_prc_copy_complete();

    irqEnable(IRQ_VBLANK);
}

IWRAM_CODE ARM_CODE
void mainloop()
{
    while (1)
    {
        //VBlankIntrWait();
        MinxCPU_Exec();
    }
}

int main()
{
    SetMode(MODE_1|BG2_ON|OBJ_ON|OBJ_1D_MAP|(option_mask_screen?WIN0_ON:0));
    irqInit();
    if (option_72hz_refresh)
        irqEnable(IRQ_TIMER3);
    irqEnable(IRQ_VBLANK);
    irqEnable(IRQ_VCOUNT);

    irqSet(IRQ_VBLANK, isr_display);
    irqSet(IRQ_TIMER3, isr_prc_copy_complete);
    irqSet(IRQ_VCOUNT, isr_vcount);

    //Setup timer 3
    REG_TM3CNT_L= -228; //-228 is Approx. 1/72 of a second
    REG_TM3CNT_H= 0b11000011; //ENABLE|IRQ|FREQ_1024;

    prc_build_palette(128);

    //Disable all sprites
    for (int i=0; i<128; i++)
    {
        OAM[i].attr0= OBJ_Y(161)|ATTR0_DISABLED;
    }

    REG_BG2CNT= ROTBG_SIZE_32|CHAR_BASE(0)|SCREEN_BASE(31)|BG_256_COLOR;

    //Set sprite affine matrix
    int sin_rot= -256;
    int cos_rot= 0;
    //No flip
    ((OBJAFFINE*)OAM)[0].pa= (128*cos_rot)>>8;
    ((OBJAFFINE*)OAM)[0].pb= (128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[0].pc= (-128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[0].pd= (128*cos_rot)>>8;
    //H flip
    ((OBJAFFINE*)OAM)[1].pa= (128*cos_rot)>>8;
    ((OBJAFFINE*)OAM)[1].pb= (128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[1].pc= (128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[1].pd= (-128*cos_rot)>>8;
    //V flip
    ((OBJAFFINE*)OAM)[2].pa= (-128*cos_rot)>>8;
    ((OBJAFFINE*)OAM)[2].pb= (-128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[2].pc= (-128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[2].pd= (128*cos_rot)>>8;
    //V & H flip
    ((OBJAFFINE*)OAM)[3].pa= (-128*cos_rot)>>8;
    ((OBJAFFINE*)OAM)[3].pb= (-128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[3].pc= (128*sin_rot)>>8;
    ((OBJAFFINE*)OAM)[3].pd= (-128*cos_rot)>>8;
    //Set background affine matrix
    REG_BG2PA= (128*cos_rot)>>8;
    REG_BG2PB= (-128*sin_rot)>>8;
    REG_BG2PC= (-128*sin_rot)>>8;
    REG_BG2PD= (-128*cos_rot)>>8;
    // REG_BG2X= 112*256;
    // REG_BG2Y= -12*256;

    //Setup screen mask
    REG_WIN0H= ((vSCREEN_XOFS)<<8)|((SCREEN_WIDTH-vSCREEN_XOFS));
    REG_WIN0V= ((vSCREEN_YOFS)<<8)|((SCREEN_HEIGHT-vSCREEN_YOFS));
    REG_WININ=  0x003F;
    REG_WINOUT= 0x002B;

    //Load IO registers default value
    for (int ir=0; ir<sizeof(PM_IO_INIT); ir++)
        minx_set_reg(ir, PM_IO_INIT[ir]);
    //Reset RAM to default value
    memset(minx_ram, 0xFF, 4096);

    // MinxCPU.PC.W.L= 0x21D0;
    MinxCPU.PC.W.L= bios_irq_vect[0];
    MinxCPU.PC.W.H= 0x0000;

    mainloop();

    return 0;
}
