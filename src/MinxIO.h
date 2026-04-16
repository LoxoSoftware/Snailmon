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

#ifndef MINXIO_H
#define MINXIO_H

#define EEPROM      ((u8*)SRAM)

typedef enum
{
    EEPROM_SEQ_IDLE=          0,
    EEPROM_SEQ_CMD=           1,
    EEPROM_SEQ_ADDR_HI=       2,
    EEPROM_SEQ_ADDR_LOW=      3,
    EEPROM_SEQ_WR_BYTE=       4,
    EEPROM_SEQ_SREAD_BYTES=   10,
} eeprom_seq_t;

typedef struct
{
    uint8_t       clock; //Used for debug purposes only
    uint8_t       bits_in;
     int8_t       bits_in_index;
    uint8_t       bits_out;
     int8_t       bits_out_index;
    eeprom_seq_t  seq_ind;
    uint8_t       cmd;
    uint16_t      addr; //Remember to align members!
    uint8_t       write_protect;
} eeprom_stat_t;

extern eeprom_stat_t* eeprom_stat;

extern int8_t lcd_contrast;
extern bool lcd_invert;

//void eeprom_send_byte(uint8_t data);

void eeprom_init();

void eeprom_send_bit(uint8_t bit, uint8_t clock);
uint8_t eeprom_receive_bit(uint8_t clock);

void lcd_send_cmd(uint8_t cmd);

#endif
