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

#include "include.h"
#include "interrupt.h"
#include "ui.h"

#define UI_TILE_INDEX       1

#define CPU_DEBUG_MEMBASE   0x1000 //Offset in EWRAM

extern TMinxCPU MinxCPU;
extern u8 minx_ram[];

uint8_t frames= 0;
bool block_vblank_irq= false;
uint8_t block_vblank_irq_frames= 0;

const u16* bios_irq_vect= (u16*)bios_bin;

uint8_t option_mask_screen= 1;
uint8_t option_72hz_refresh= 0;
uint8_t option_thread_safety= 1; //0= Off, 1= Fast, 2= Safe
uint8_t option_cart_rtc= 1;

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

#pragma GCC push_options
#pragma GCC optimize ("Os")

IWRAM_CODE ARM_CODE
void isr_prc_copy_complete()
{
    if (block_vblank_irq_frames < 3) //Prevent deadlock
    if (block_vblank_irq && option_thread_safety)
    {
        block_vblank_irq= false;
        block_vblank_irq_frames++;
        return;
    }
    block_vblank_irq_frames= 0;

    if (frames&1)
    {
        send_irq(VIRQ_PRC_COPY_DONE);
        send_irq(VIRQ_PRC_FRAMEDIV_OF);
    }

    frames++;
}

void go_menu();

IWRAM_CODE ARM_CODE
void isr_display()
{
    irqDisable(IRQ_VBLANK);
    irqDisable(IRQ_TIMER3);

    REG_BG2X= ((-(vSCREEN_YOFS>>1)+(MinxRegs[VREG_PRC_SCROLL_Y]&0x7F))<<8);
    REG_BG2Y= (-(vSCREEN_XOFS>>1)+(MinxRegs[VREG_PRC_SCROLL_X]&0x7F))<<8;

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

    scanKeys();
    u16 kd= keysDown();

    //Toggle screen mask
    if (kd&KEY_SELECT)
    {
        // option_mask_screen= !option_mask_screen;
        // SetMode(MODE_1|BG2_ON|OBJ_ON|OBJ_1D_MAP|(option_mask_screen?WIN0_ON:0));
        // prc_pending_updates |= PRC_QUEUE_COPY_BG_GFX | PRC_QUEUE_COPY_SPR_GFX | PRC_QUEUE_FORCE_UPDATE;
        go_menu();
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
        //send_irq(VIRQ_PRC_FRAMEDIV_OF);
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

    if (MinxRegs[VREG_PRC_MODE] != MinxRegs[VREG_PRC_MODE_OLD])
    {
        prc_on_mode_set();
        MinxRegs[VREG_PRC_MODE_OLD]= MinxRegs[VREG_PRC_MODE];
    }

    if (frames >= 59)
    {
        reg_sec_counter_increment();
        frames= 0;
    }

    if (!option_72hz_refresh)
        isr_prc_copy_complete();
    else
        irqEnable(IRQ_TIMER3);

    irqEnable(IRQ_VBLANK);
}

#pragma GCC pop_options

void ui_init()
{
    memcpy((u16*)TILE_BASE_ADR(3), uiTiles, uiTilesLen);
    memcpy((u16*)BG_PALETTE+0xF0, uiPal, 32);
    REG_BG0CNT= BG_16_COLOR | BG_SIZE_0 | BG_TILE_BASE(3) | BG_MAP_BASE(30) | BG_PRIORITY(0);
    REG_DISPCNT |= BG0_ENABLE;
}

void ui_clear()
{
    for (int i=0; i<32*32; i++)
        ((u16*)MAP_BASE_ADR(30))[i]= 0xF000;
}

void ui_draw_frame(int x, int y, int w, int h)
{
    for (int iy=y; iy<y+h+2; iy++)
    {
        for (int ix=x; ix<x+w+2; ix++)
        {
            u16* tm= &((u16*)MAP_BASE_ADR(30))[ix+iy*32];

            if (iy==y || iy==y+h+1)
                *tm= 0xF003;
            else if (ix==x)
                *tm= 0xF005;
            else if (ix==x+w+1)
                *tm= 0xF006;
            else
                *tm= 0xF001;
            if (ix==x && iy==y)
                *tm= 0xF002;
            else if (ix==x+w+1 && iy==y)
                *tm= 0xF004;
            else if (ix==x && iy==y+h+1)
                *tm= 0xF007;
            else if (ix==x+w+1 && iy==y+h+1)
                *tm= 0xF009;
        }
    }
}

void ui_draw_char(int x, int y, char ch)
{
    ((u16*)MAP_BASE_ADR(30))[x+y*32]= 0xF000|ch;
}

void ui_draw_string(int x, int y, const char* str)
{
    int str_len= (int)(strchr(str, '#')-str);//strlen(str);

    for (int i=0; i<str_len; i++)
        ui_draw_char(x+i, y, str[i]);
}

void go_menu()
{
    int sel= 0;
    const int options= 5;

    ui_init();
    ui_clear();
    ui_draw_frame(0,0,20,options); //Main menu
    ui_draw_frame(0,options+2,20,1); //Sleep notice
    ui_draw_string(1,options+3, "Press START to sleep#");

    const char* menu_elems[]= {
        "MASK DISPLAY # OFF#  ON#",
        "PALETTE      # LCD# B&W#DMG1#DMG2#",
        "FRAME DIVIDER#30HZ#36HZ#",
        "THREAD SAFETY# OFF#FAST#SAFE#",
        "CARTRIDGE RTC# OFF#  ON#",
    };
    u8* menu_varptr[]=
    {
        &option_mask_screen,
        &MinxRegs[VREG_PRC_PALETTE],
        &option_72hz_refresh,
        &option_thread_safety,
        &option_cart_rtc,
    };
    const int menu_choices_len[]=
    {
        2,
        4,
        2,
        3,
        2,
    };
    int menu_elems_len= sizeof(menu_elems)/sizeof(char*);

    void draw_menu(int x, int y, int sel)
    {
        int opt_len= (int)(strchr(menu_elems[0], '#')-menu_elems[0]);
        int val_str_len= 5;//(int)(strchr(menu_elems[0]+opt_len+1, '#')-menu_elems[0]+opt_len+1);

        for (int im=0; im<menu_elems_len; im++)
        {
            ui_draw_char(x, y+im, (sel==im)?0x0B:0x0A);
            ui_draw_string(x+1, y+im, menu_elems[im]);
            ui_draw_string(x+1+opt_len+2, y+im, (char*)(menu_elems[im]+opt_len+1+(val_str_len)*(*menu_varptr[im])));
        }
    }

    draw_menu(1,1, sel);

    scanKeys();
    u16 kd= keysDown();

    while(1)
    {
        if (kd&KEY_UP && sel>0)
            draw_menu(1,1, --sel);
        if (kd&KEY_DOWN && sel<menu_elems_len-1)
            draw_menu(1,1, ++sel);
        if (kd&KEY_A)
        {
            if (*menu_varptr[sel]+1<menu_choices_len[sel])
                (*menu_varptr[sel])++;
            else
                (*menu_varptr[sel])= 0;
            draw_menu(1,1, sel);
        }
        if (kd&KEY_START)
        {
            MinxCPU_OnSleep(0);
            break;
        }
        if (kd&KEY_SELECT || kd&KEY_B)
            break;

        scanKeys();
        kd= keysDown();
    }

    ui_clear();

    //Force reload of tile gfx data
    prc_pending_updates |= PRC_QUEUE_COPY_BG_GFX | PRC_QUEUE_COPY_SPR_GFX | PRC_QUEUE_FORCE_UPDATE;
    //Apply mask setting
    REG_DISPCNT= REG_DISPCNT&(~WIN0_ON);
    if (option_mask_screen) REG_DISPCNT |= WIN0_ON;
    //Rebuild palette
    prc_build_palette();
}

uint8_t FetchCode_debug(uint32_t addr)
{
	if (addr&0xFFF000)
		return rom_bin[addr&0x1FFFFF];
	else
		return bios_bin[addr&0x0FFF];
}

void cpu_debug_print()
{
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE  ]= MinxCPU.PC.B.L;
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+1 ]= MinxCPU.PC.B.H;
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+2 ]= MinxCPU.PC.B.I;
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+3 ]= MinxCPU.PC.B.X;
    uint32_t rom_fetch_addr;
    if (MinxCPU.PC.W.L & 0x8000) {
		// Banked area
		rom_fetch_addr= (MinxCPU.PC.W.L & 0x7FFF)|(MinxCPU.PC.B.I << 15);
	} else {
		// Unbanked area
		rom_fetch_addr= MinxCPU.PC.W.L;
	}

    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE]++;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+1 ]= rom_fetch_addr>>16;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+2 ]= (rom_fetch_addr>>8)&0xFF;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+3 ]= rom_fetch_addr&0xFF;
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+4 ]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L));
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+5 ]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L)+1);
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+6 ]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L)+2);
    // ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+7 ]= MinxCPU_OnRead(0, ((MinxCPU.PC.W.H<<16)|MinxCPU.PC.W.L)+3);
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+4 ]= FetchCode_debug(rom_fetch_addr);
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+5 ]= FetchCode_debug(rom_fetch_addr+1);
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+6 ]= FetchCode_debug(rom_fetch_addr+2);
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+7 ]= FetchCode_debug(rom_fetch_addr+3);
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+8 ]= MinxCPU.HL.B.L;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+9 ]= MinxCPU.HL.B.H;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+10]= MinxCPU.HL.B.I;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+11]= MinxCPU.HL.B.X;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+12]= MinxCPU.SP.B.L;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+13]= MinxCPU.SP.B.H;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+14]= MinxCPU.SP.B.I;
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE+15]= MinxCPU.SP.B.X;
    //*((u32*)0x2001010)= (u32)rom_bin;
}

IWRAM_CODE ARM_CODE
void mainloop()
{
    ((u8*)EWRAM)[CPU_DEBUG_MEMBASE]= 0x00; //Cycles counter (works in DEBUG mode)

    while (1)
    {
        //VBlankIntrWait();
        MinxCPU_Exec();

#if MINXCPU_DEBUG == 1
        cpu_debug_print();
#endif
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

    eeprom_init(); //Does not clear the eeprom

    //Setup timer 3
    REG_TM3CNT_L= -228; //-228 is Approx. 1/72 of a second
    REG_TM3CNT_H= 0b11000011; //ENABLE|IRQ|FREQ_1024;

    prc_build_palette();

    //Disable all sprites
    for (int i=0; i<128; i++)
    {
        OAM[i].attr0= OBJ_Y(161)|ATTR0_DISABLED;
    }

    REG_BG2CNT= ROTBG_SIZE_32|CHAR_BASE(0)|SCREEN_BASE(31)|BG_256_COLOR|BG_PRIORITY(3)|BG_WRAP;

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
