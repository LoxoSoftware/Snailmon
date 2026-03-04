#ifndef MINXPPU_H
#define MINXPPU_H

#include <stdio.h>
#include <stdint.h>

#define vSCREEN_WIDTH   96
#define vSCREEN_HEIGHT  64

// #define vSCREEN_XOFS 24
// #define vSCREEN_YOFS 16

//#define PRC_GET_TILE_LINE(tile, y) (tile[y])

typedef struct
{
    uint16_t* host_fb;
    uint16_t  host_fb_w;
    uint8_t line_cnt;
} TMinxPRC;

extern TMinxPRC MinxPRC;
extern uint8_t minx_ram[];

void prc_build_palette(int contrast);

void host_vram_write(uint32_t ofs, uint8_t data);
void host_vram_write_4bpp(uint32_t ofs, uint8_t data);
void host_vram_write_sprrow_4bpp(uint32_t ofs, uint8_t shade, uint8_t mask);

void isr_sprduplex_fcopy();
void isr_sprduplex_maprender();

void prc_on_map_addr_change();
void prc_on_spr_addr_change();
void prc_on_oam_update(int sprid);

#endif
