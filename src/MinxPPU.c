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

#define GFX_MAP_CHR_ADR ((u16*)CHAR_BASE_ADR(0))

extern uint8_t minx_ram[];

const uint16_t lut_mapw[4]= { 12, 16, 24, 24 };
const uint16_t lut_maph[4]= { 16, 12, 8, 16 };
const uint16_t lut_prc_map_bytes[4]= { 12*16, 16*12, 24*8, 24*16 };
//const uint16_t lut_bg_xofs[4]= { 144, 112, 112, 112 }; //Just to center the screen

void prc_build_palette(int contrast)
{
    if (contrast >= 0)
    {
        BG_PALETTE[1]= RGB5(4,6,4);
        BG_PALETTE[0]= RGB5(24,28,24);
        SPRITE_PALETTE[2]= RGB5(24,28,24);
        SPRITE_PALETTE[1]= RGB5(4,6,4);
        SPRITE_PALETTE[0]= RGB5(31,0,31);
    }
    else
    {
        BG_PALETTE[0]= RGB5(4,6,4);
        BG_PALETTE[1]= RGB5(24,28,24);
        SPRITE_PALETTE[1]= RGB5(24,28,24);
        SPRITE_PALETTE[2]= RGB5(4,6,4);
        SPRITE_PALETTE[0]= RGB5(31,0,31);
    }
}

// void host_vram_write(uint32_t ofs, uint8_t data)
// {
//     ((u16*)SPRITE_GFX)[ofs*4+3]= (((data>>0)&1)<<8)|((data>>1)&1),
//     ((u16*)SPRITE_GFX)[ofs*4+2]= (((data>>2)&1)<<8)|((data>>3)&1),
//     ((u16*)SPRITE_GFX)[ofs*4+1]= (((data>>4)&1)<<8)|((data>>5)&1),
//     ((u16*)SPRITE_GFX)[ofs*4]= (((data>>6)&1)<<8)|((data>>7)&1);
// }

void host_vram_write(uint32_t ofs, uint8_t data)
{
    GFX_MAP_CHR_ADR[ofs*4+3]= (((data>>0)&1)<<8)|((data>>1)&1),
    GFX_MAP_CHR_ADR[ofs*4+2]= (((data>>2)&1)<<8)|((data>>3)&1),
    GFX_MAP_CHR_ADR[ofs*4+1]= (((data>>4)&1)<<8)|((data>>5)&1),
    GFX_MAP_CHR_ADR[ofs*4]= (((data>>6)&1)<<8)|((data>>7)&1);
}

inline void host_vram_write_4bpp(uint32_t ofs, uint8_t data)
{
    ((u16*)SPRITE_GFX)[ofs*2+1]= (((data)&1)<<12)
                                |(((data>>1)&1)<<8)
                                |(((data>>2)&1)<<4)
                                |((data>>3)&1);
    ((u16*)SPRITE_GFX)[ofs*2]= (((data>>4)&1)<<12)
                                |(((data>>5)&1)<<8)
                                |(((data>>6)&1)<<4)
                                |((data>>7)&1);
}

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
    // ((u16*)SPRITE_GFX)[ofs*2+1]= (((shade)&(~(mask)&0x01))<<12)
    //                             |(((shade>>1)&(~(mask>>1)&0x01))<<8)
    //                             |(((shade>>2)&(~(mask>>2)&0x01))<<4)
    //                             |(((shade>>3)&(~(mask>>3)&0x01)));
    // ((u16*)SPRITE_GFX)[ofs*2]= (((shade>>4)&(~(mask>>4)&0x01))<<12)
    //                             |(((shade>>5)&(~(mask>>5)&0x01))<<8)
    //                             |(((shade>>6)&(~(mask>>6)&0x01))<<4)
    //                             |(((shade>>7)&(~(mask>>7)&0x01)));
}

IWRAM_CODE ARM_CODE
void isr_sprduplex_fcopy()
{
    register int sl= REG_VCOUNT-vSCREEN_YOFS;
    int row= sl>>4;

    for (int ix=0; ix<12; ix++)
    {
        OAM[GFX_BG_SPRID+ix].attr0= OBJ_Y(REG_VCOUNT)|ATTR0_COLOR_256|ATTR0_ROTSCALE_DOUBLE;
        OAM[GFX_BG_SPRID+ix].attr1= OBJ_X((ix<<4)+vSCREEN_XOFS)|ATTR1_ROTDATA(0)|ATTR1_SIZE_8;
        OAM[GFX_BG_SPRID+ix].attr2= OBJ_CHAR((ix+12*row)<<1);
    }

    if (sl >= 112)
        REG_DISPSTAT= (REG_DISPSTAT&0xFF)|(vSCREEN_YOFS<<8);
    else
        REG_DISPSTAT= (REG_DISPSTAT&0xFF)|((REG_VCOUNT+16)<<8);
}

IWRAM_CODE ARM_CODE
void isr_sprduplex_maprender()
{
    register int sl= REG_VCOUNT-vSCREEN_YOFS;
    register int yscroll= MinxRegs[VREG_PRC_SCROLL_Y]<<1;
//     int row= (sl+yscroll)>>4;
//
//     for (int ix=0; ix<12; ix++)
//     {
// #ifdef GFX_8BPP
//         OAM[GFX_BG_SPRID+ix].attr0= OBJ_Y(REG_VCOUNT)|ATTR0_COLOR_256|ATTR0_ROTSCALE_DOUBLE;
//         OAM[GFX_BG_SPRID+ix].attr2= OBJ_CHAR((minx_ram[0x360+ix+lut_mapw[(MinxRegs[VREG_PRC_MODE]>>PRC_MODE_MAP_SZ)]*row])<<1);
// #else
//         OAM[GFX_BG_SPRID+ix].attr0= OBJ_Y(REG_VCOUNT)|ATTR0_COLOR_16|ATTR0_ROTSCALE_DOUBLE;
//         OAM[GFX_BG_SPRID+ix].attr2= OBJ_CHAR((minx_ram[0x360+ix+lut_mapw[(MinxRegs[VREG_PRC_MODE]>>PRC_MODE_MAP_SZ)]*row]));
// #endif
//         OAM[GFX_BG_SPRID+ix].attr1= OBJ_X((ix<<4)+vSCREEN_XOFS-(MinxRegs[VREG_PRC_SCROLL_X]&0x0F))|ATTR1_ROTDATA(0)|ATTR1_SIZE_8;
//     }

    if (sl >= 112-(yscroll&0x0F))
        REG_DISPSTAT= (REG_DISPSTAT&0xFF)|(((vSCREEN_YOFS-(yscroll&0x0F))<<8));
    else
        REG_DISPSTAT= (REG_DISPSTAT&0xFF)|((REG_VCOUNT+16)<<8);

    MinxRegs[VREG_PRC_CNT]= sl>>1;
}

void prc_on_map_addr_change()
{
    int tofs=   ((MinxRegs[VREG_PRC_MAP_LO])|
                (MinxRegs[VREG_PRC_MAP_MID]<<8)|
                (MinxRegs[VREG_PRC_MAP_HI]<<16));

    for (int i=0; i<GFX_COPY_SZ; i++)
    {
        host_vram_write(i, MinxCPU_OnRead(0, tofs+i));
    }
}

void prc_on_spr_addr_change()
{
    int tofs=   ((MinxRegs[VREG_PRC_SPR_LO])|
                (MinxRegs[VREG_PRC_SPR_MID]<<8)|
                (MinxRegs[VREG_PRC_SPR_HI]<<16));

    for (int it=0, im=0; it<GFX_COPY_SZ>>1; it+=4) //For every sprite tile
    {
        for (int ir=0; ir<16; ir++, im++) //For every 2 tiles rows
        {
            uint8_t row_mask= MinxCPU_OnRead(0, tofs+it*8+ir);
            uint8_t row_shade= MinxCPU_OnRead(0, tofs+(it+2)*8+ir);

            switch ((im>>3)&3) //Fix tile order
            {
                case 0:
                    host_vram_write_sprrow_4bpp(GFX_COPY_SZ+(im&0xFFE7)+8, ~row_shade, row_mask);
                    break;
                case 1:
                    host_vram_write_sprrow_4bpp(GFX_COPY_SZ+(im&0xFFE7), ~row_shade, row_mask);
                    break;
                case 2:
                    host_vram_write_sprrow_4bpp(GFX_COPY_SZ+(im&0xFFE7)+24, ~row_shade, row_mask);
                    break;
                case 3:
                    host_vram_write_sprrow_4bpp(GFX_COPY_SZ+(im&0xFFE7)+16, ~row_shade, row_mask);
                    break;
            }
        }
    }
}

void prc_on_oam_update(int sprid)
{
    uint8_t* spr_oamptr= &minx_ram[0x300+sprid*4];

    if (spr_oamptr[3]&PRC_OAM3_ENABLE)
    {
        OAM[GFX_SPR_SPRID+sprid].attr0= OBJ_Y((spr_oamptr[1]<<1)+vSCREEN_YOFS-32)|ATTR0_COLOR_16|ATTR0_SQUARE|ATTR0_ROTSCALE_DOUBLE;
        OAM[GFX_SPR_SPRID+sprid].attr2= OBJ_CHAR(256+(spr_oamptr[2]&0x7F)*4);
        OAM[GFX_SPR_SPRID+sprid].attr1= OBJ_X((spr_oamptr[0]<<1)+vSCREEN_XOFS-32)|ATTR1_SIZE_16;
    }
    else
        OAM[GFX_SPR_SPRID+sprid].attr0= OBJ_Y(180)|ATTR0_DISABLED;
}

#endif
