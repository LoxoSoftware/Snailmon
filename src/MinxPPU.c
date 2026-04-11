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

#ifndef MINXPPU_H
#define MINXPPU_H

#include <stdio.h>
#include <stdint.h>
#include <gba.h>
#include "include.h"

#define vSCREEN_XOFS    24
#define vSCREEN_YOFS    16

#define GFX_COPY_SZ     2048
#define GFX_SPR_SPRID   16
#define GFX_BG_SPRID    100

#define PRC_OAM3_VFLIP  (1)
#define PRC_OAM3_HFLIP  (1<<1)
#define PRC_OAM3_INVERT (1<<2)
#define PRC_OAM3_ENABLE (1<<3)

#define GFX_MAP_CHR_ADR ((u16*)CHAR_BASE_ADR(prc_bg_tile_base))
#define GFX_MAP_SCR_ADR ((u16*)MAP_BASE_ADR(31))

extern uint8_t minx_ram[];
extern bool block_vblank_irq;
extern uint8_t option_thread_safety;

const uint16_t lut_mapw[4]= { 12, 16, 24, 24 };
const uint16_t lut_maph[4]= { 16, 12, 8, 16 };
const uint16_t lut_prc_map_bytes[4]= { 12*16, 16*12, 24*8, 24*16 };
//Sprite position correction due to PPU inaccuracy
const int16_t  lut_spr_xcorr[4]= { 0, -1, 0, -1 }; //No flip | H flip | V flip | both flip
const int16_t  lut_spr_ycorr[4]= { -1, -1, 0, 0 }; //No flip | H flip | V flip | both flip

uint8_t prc_pending_updates= 0x00;
uint32_t prc_bg_tile_base_cached[2]= { 0xFFFFFFFF, 0xFFFFFFFF };
uint32_t prc_spr_tile_base_cached[2]= { 0xFFFFFFFF, 0xFFFFFFFF };
uint8_t prc_bg_tile_base= 0;
uint8_t prc_spr_tile_base= 0;

typedef struct
{
    int8_t r;
    int8_t g;
    int8_t b;
} rgb_t;

const rgb_t palette_low[4]= {
    (rgb_t){ 4,  6, 4 }, //LCD  palette
    (rgb_t){ 0,  0, 0 }, //B&W  palette
    (rgb_t){ 4,  8, 0 }, //DMG1 palette
    (rgb_t){ 3, 10, 6 }, //DMG2 palette
};
const rgb_t palette_high[4]= {
    (rgb_t){ 24, 26, 24 }, //LCD  palette
    (rgb_t){ 31, 31, 31 }, //B&W  palette
    (rgb_t){ 30, 31,  0 }, //DMG1 palette
    (rgb_t){ 20, 31,  8 }, //DMG2 palette
};
const rgb_t palette_bg[4]= {
    (rgb_t){ 24, 26, 24 }, //LCD  palette
    (rgb_t){ 31, 31, 31 }, //B&W  palette
    (rgb_t){ 30, 31,  0 }, //DMG1 palette
    (rgb_t){ 31, 26,  6 }, //DMG2 palette
};

void prc_build_palette()
{
    //Contrast is from 0 to 64 with a negative value representing inversion state
    uint8_t pal= MinxRegs[VREG_PRC_PALETTE]&3;

    int new_contrast= lcd_contrast/1.5;

    if (!lcd_invert)
    {
        rgb_t new_low= (rgb_t)
        {
            palette_high[pal].r-(new_contrast),
            palette_high[pal].g-(new_contrast),
            palette_high[pal].b-(new_contrast)
        };

        BG_PALETTE[0x02]= RGB5((new_low.r<palette_low[pal].r)? palette_low[pal].r : new_low.r,
                               (new_low.g<palette_low[pal].g)? palette_low[pal].g : new_low.g,
                               (new_low.b<palette_low[pal].b)? palette_low[pal].b : new_low.b);
        BG_PALETTE[0x01]= RGB5(palette_high[pal].r, palette_high[pal].g, palette_high[pal].b);
    }
    else
    {
        rgb_t new_high= (rgb_t)
        {
            palette_low[pal].r+(new_contrast),
            palette_low[pal].g+(new_contrast),
            palette_low[pal].b+(new_contrast)
        };

        BG_PALETTE[0x01]= RGB5(palette_low[pal].r, palette_low[pal].g, palette_low[pal].b);
        BG_PALETTE[0x02]= RGB5((new_high.r>palette_high[pal].r)? palette_high[pal].r : new_high.r,
                               (new_high.g>palette_high[pal].g)? palette_high[pal].g : new_high.g,
                               (new_high.b>palette_high[pal].b)? palette_high[pal].b : new_high.b);
    }

    if (pal == 3) //DMG2 palette has a different BG color
        BG_PALETTE[0x00]= RGB5(palette_bg[pal].r, palette_bg[pal].g, palette_bg[pal].b);
    else
        BG_PALETTE[0x00]= BG_PALETTE[0x01];
    SPRITE_PALETTE[0x00]= RGB5(31,0,31);
    SPRITE_PALETTE[0x01]= BG_PALETTE[0x02];
    SPRITE_PALETTE[0x02]= BG_PALETTE[0x01];
    SPRITE_PALETTE[0x10]= RGB5(0,31,0);
    SPRITE_PALETTE[0x11]= BG_PALETTE[0x01];
    SPRITE_PALETTE[0x12]= BG_PALETTE[0x02];
}

IWRAM_CODE ARM_CODE
void host_vram_write(register uint32_t ofs, register uint8_t data)
{
    GFX_MAP_CHR_ADR[(ofs<<2)+3]= ((((data>>7)&1)+1)<<8)|(((data>>6)&1)+1),
    GFX_MAP_CHR_ADR[(ofs<<2)+2]= ((((data>>5)&1)+1)<<8)|(((data>>4)&1)+1),
    GFX_MAP_CHR_ADR[(ofs<<2)+1]= ((((data>>3)&1)+1)<<8)|(((data>>2)&1)+1),
    GFX_MAP_CHR_ADR[(ofs<<2)]= ((((data>>1)&1)+1)<<8)|(((data)&1)+1);
}

IWRAM_CODE ARM_CODE
void host_vram_write_sprrow_4bpp(uint32_t ofs, uint8_t shade, uint8_t mask)
{
    ((u16*)SPRITE_GFX)[ofs*2+1]= ((mask&0x01)?0:((shade)&1)+1)<<12
                                |((mask&0x02)?0:((shade>>1)&1)+1)<<8
                                |((mask&0x04)?0:((shade>>2)&1)+1)<<4
                                |((mask&0x08)?0:((shade>>3)&1)+1);
    ((u16*)SPRITE_GFX)[ofs*2]= ((mask&0x10)?0:((shade>>4)&1)+1)<<12
                                |((mask&0x20)?0:((shade>>5)&1)+1)<<8
                                |((mask&0x40)?0:((shade>>6)&1)+1)<<4
                                |((mask&0x80)?0:((shade>>7)&1)+1);
}

void host_map_write(uint16_t ofs, uint8_t data)
{
    if (!(MinxRegs[VREG_PRC_MODE]&PRC_MODE_ENA_MAP))
        return;

    //uint16_t map_rows= lut_maph[MinxRegs[VREG_PRC_MODE]>>4];
    uint16_t map_cols= lut_mapw[MinxRegs[VREG_PRC_MODE]>>4];
    uint16_t tile_row= (ofs/map_cols);
    uint16_t tile_col= (ofs%map_cols);
    uint16_t* vram_addr= &GFX_MAP_SCR_ADR[(tile_row+tile_col*32)>>1];
    if (tile_row&1)
        *vram_addr= (data<<8)|(*vram_addr&0x00FF);
    else
        *vram_addr= (*vram_addr&0xFF00)|data;
}

IWRAM_CODE ARM_CODE
void isr_vcount()
{
    if (REG_VCOUNT >= 128)
        REG_DISPSTAT= (REG_DISPSTAT&0xFF)|((0)<<8);
    else
        REG_DISPSTAT= (REG_DISPSTAT&0xFF)|((REG_VCOUNT+16)<<8);

    MinxRegs[VREG_PRC_CNT]= (REG_VCOUNT>>1)+1;

    if (option_thread_safety == 1)
        block_vblank_irq= 0;
}

IWRAM_CODE ARM_CODE
void prc_on_map_addr_change()
{
    REG_IME= 0;

    int tofs=   ((MinxRegs[VREG_PRC_MAP_LO])|
                (MinxRegs[VREG_PRC_MAP_MID]<<8)|
                (MinxRegs[VREG_PRC_MAP_HI]<<16));

    //Check if the tiles are already cached
    for (int ib=0; ib<2; ib++)
    {
        if (prc_bg_tile_base_cached[ib] == tofs)
        {
            prc_bg_tile_base= ib;
            //NOTE: force update is not supported by this code
            goto prc_bg_tile_copy_done;
        }
    }
    //If this point is reached tiles were not cached
    prc_bg_tile_base= (prc_bg_tile_base+1)&1;
    prc_bg_tile_base_cached[prc_bg_tile_base]= tofs;

//prc_bg_tile_copy_begin:

    for (int i=0; i<GFX_COPY_SZ; i++)
    {
        host_vram_write(i, MinxCPU_OnRead(0, tofs+i));
    }

prc_bg_tile_copy_done:

    REG_BG2CNT= (REG_BG2CNT&0b1111111111110011)|CHAR_BASE(prc_bg_tile_base);

    prc_pending_updates &= 0b11111110;
    REG_IME= 1;
}

void prc_on_oam_update(int sprid);
void prc_on_spr_addr_change()
{
    REG_IME= 0;

    int tofs=   ((MinxRegs[VREG_PRC_SPR_LO])|
                (MinxRegs[VREG_PRC_SPR_MID]<<8)|
                (MinxRegs[VREG_PRC_SPR_HI]<<16));

#if GFX_SPR_FULLSET == 0
    //Check if the tiles are already cached
    for (int ib=0; ib<2; ib++)
    {
        if (prc_spr_tile_base_cached[ib] == tofs)
        {
            prc_spr_tile_base= ib;
            if (prc_pending_updates&4)
            {
                //Graphics force update request
                goto prc_spr_tile_copy_begin;
            }
            goto prc_spr_tile_copy_done;
        }
    }
    //If this point is reached tiles were not cached

    prc_spr_tile_base= (prc_spr_tile_base+1)&1;
#else
    if (prc_spr_tile_base_cached[0] == tofs)
        //Graphics already in memory
        goto prc_spr_tile_copy_done;

    prc_spr_tile_base= 0;
#endif
    prc_spr_tile_base_cached[prc_spr_tile_base]= tofs;

#if GFX_SPR_FULLSET == 1
    for (int it=0, im=0; it<GFX_COPY_SZ; it+=4) //For every sprite tile
#else
    prc_spr_tile_copy_begin:

    for (int it=0, im=(GFX_COPY_SZ>>1)*prc_spr_tile_base*4; it<(GFX_COPY_SZ>>1); it+=4) //For every sprite tile
#endif
    {
        for (int ir=0; ir<16; ir++, im++) //For every 2 tiles rows
        {
            uint8_t row_mask= MinxCPU_OnRead(0, tofs+it*8+ir);
            uint8_t row_shade= MinxCPU_OnRead(0, tofs+(it+2)*8+ir);

            switch ((im>>3)&3) //Fix tile order
            {
                case 0:
                    host_vram_write_sprrow_4bpp((im&0xFFFFE7)+8, ~row_shade, row_mask);
                    break;
                case 1:
                    host_vram_write_sprrow_4bpp((im&0xFFFFE7), ~row_shade, row_mask);
                    break;
                case 2:
                    host_vram_write_sprrow_4bpp((im&0xFFFFE7)+24, ~row_shade, row_mask);
                    break;
                case 3:
                    host_vram_write_sprrow_4bpp((im&0xFFFFE7)+16, ~row_shade, row_mask);
                    break;
            }
        }
    }

prc_spr_tile_copy_done:
#if GFX_SPR_FULLSET == 0

    for (int i=0; i<24; i++)
        prc_on_oam_update(i);
#endif

    prc_pending_updates &= 0b11111001;
    REG_IME= 1;
}

IWRAM_CODE ARM_CODE
void prc_on_oam_update(int sprid)
{
    uint8_t* spr_oamptr= &minx_ram[0x300+sprid*4];
    uint8_t flip_mode= spr_oamptr[3]&0x03;

    if (spr_oamptr[3]&PRC_OAM3_ENABLE)
    {
        OAM[GFX_SPR_SPRID+sprid].attr0= OBJ_Y((spr_oamptr[1]<<1)+vSCREEN_YOFS-32+lut_spr_ycorr[flip_mode])|ATTR0_COLOR_16|ATTR0_SQUARE|ATTR0_ROTSCALE_DOUBLE;
#if GFX_SPR_FULLSET == 1
        OAM[GFX_SPR_SPRID+sprid].attr2= OBJ_PALETTE((spr_oamptr[3]&PRC_OAM3_INVERT)?1:0)|OBJ_CHAR((prc_spr_tile_base*1024)+(spr_oamptr[2])*4)|ATTR2_PRIORITY(1);
#else
        OAM[GFX_SPR_SPRID+sprid].attr2= OBJ_PALETTE((spr_oamptr[3]&PRC_OAM3_INVERT)?1:0)|OBJ_CHAR((prc_spr_tile_base*512)+(spr_oamptr[2]&0x7F)*4)|ATTR2_PRIORITY(1);
#endif
        OAM[GFX_SPR_SPRID+sprid].attr1= OBJ_ROT_SCALE(flip_mode)|OBJ_X((spr_oamptr[0]<<1)+vSCREEN_XOFS-32+lut_spr_xcorr[flip_mode])|ATTR1_SIZE_16;
    }
    else
        OAM[GFX_SPR_SPRID+sprid].attr0= OBJ_Y(180)|ATTR0_DISABLED;
}

void prc_on_map_update()
{
    //Copy all of map data from PM to GBA
    for (int i=0; i<lut_prc_map_bytes[MinxRegs[VREG_PRC_MODE]>>4]; i++)
        host_map_write(i, minx_ram[0x360+i]);
}

void prc_on_fcopy_mode()
{
    //Make map for the host background layer
    for (int iy=0; iy<12; iy++)
    {
        for (int ix=0; ix<8; ix+=2)
            ((u16*)MAP_BASE_ADR(31))[(ix+iy*32)>>1]= (iy+ix*12)|((iy+(ix+1)*12)<<8);
    }
}

void prc_on_mode_set()
{
    uint8_t data= MinxRegs[VREG_PRC_MODE];

    switch ((data>>1)&0x07)
    {
        case 0b000: case 0b010: case 0b001: case 0b011:
            //PRC disabled (no frame copy)
            //irqDisable(IRQ_VCOUNT);
            //REG_DISPCNT= (REG_DISPCNT&0xFF7F)|LCDC_OFF;
            REG_DISPCNT= (REG_DISPCNT&0xEB7F);
            break;
        case 0b101: case 0b111:
            //Map rendering enabled
            irqEnable(IRQ_VCOUNT);
            REG_DISPCNT= (REG_DISPCNT&0xEB7F)|BG2_ON|(data&0x4?OBJ_ON:0);
            prc_on_map_update();
            break;
        case 0b100: case 0b110:
            //Map rendering disabled
            irqEnable(IRQ_VCOUNT);
            REG_DISPCNT= (REG_DISPCNT&0xEB7F)|BG2_ON|(data&0x4?OBJ_ON:0);
            prc_on_fcopy_mode();
            break;
    }
}

#endif
