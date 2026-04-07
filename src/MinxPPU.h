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

#define vSCREEN_WIDTH   96
#define vSCREEN_HEIGHT  64

#define GFX_MAP_SCR_ADR ((u16*)MAP_BASE_ADR(31))

#define PRC_QUEUE_COPY_BG_GFX   (1<<0)
#define PRC_QUEUE_COPY_SPR_GFX  (1<<1)
#define PRC_QUEUE_FORCE_UPDATE  (1<<2)
#define PRC_QUEUE_WAIT          (1<<3)

// #define vSCREEN_XOFS 24
// #define vSCREEN_YOFS 16

extern uint8_t minx_ram[];

extern uint16_t lut_mapw[4];
extern uint16_t lut_maph[4];
extern uint16_t lut_prc_map_bytes[4];
extern uint16_t lut_bg_xofs[4];

extern uint8_t prc_pending_updates;
extern uint32_t prc_map_tile_base_cached[2];
extern uint32_t prc_spr_tile_base_cached[2];

void prc_build_palette(int contrast);

void host_vram_write(uint32_t ofs, uint8_t data);
void host_vram_write_4bpp(uint32_t ofs, uint8_t data);
void host_vram_write_sprrow_4bpp(uint32_t ofs, uint8_t shade, uint8_t mask);
void host_map_write(uint16_t ofs, uint8_t data);

void isr_vcount();

void prc_on_map_addr_change();
void prc_on_spr_addr_change();
void prc_on_oam_update(int sprid);
void prc_on_map_update();
void prc_on_fcopy_mode();

#endif
