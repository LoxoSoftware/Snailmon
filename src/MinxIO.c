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

#ifndef EEPROM
#define EEPROM ((u8*)SRAM)
#endif

eeprom_stat_t* eeprom_stat= (eeprom_stat_t*)(EWRAM+0x1200);

uint8_t lcd_cmd= 0x00;
 int8_t lcd_contrast= 0x3F;
   bool lcd_invert= false;

void eeprom_init()
{
    eeprom_stat->clock= 0;
    eeprom_stat->bits_in= 0;
    eeprom_stat->bits_in_index= 0;
    eeprom_stat->bits_out= 0;
    eeprom_stat->bits_out_index= 0;
    eeprom_stat->seq_ind= 0;
    eeprom_stat->cmd= 0;
    eeprom_stat->addr= 0;
    eeprom_stat->write_protect= 0;
}

inline uint8_t eeprom_raw_read(uint16_t ofs)
{
    return EEPROM[ofs&0x1FFF];
}

inline void eeprom_raw_write(uint16_t ofs, uint8_t data)
{
    EEPROM[ofs&0x1FFF]= data;
}

uint8_t eeprom_receive_bit(uint8_t clock);
void eeprom_send_byte(uint8_t data)
{
    switch (eeprom_stat->seq_ind)
    {
        case EEPROM_SEQ_CMD:
            //Enter CMD
            eeprom_stat->cmd= data;
            if ((eeprom_stat->cmd&0xF0) != 0xA0) //Check control sequence
            {
                eeprom_stat->seq_ind= EEPROM_SEQ_IDLE;
                return;
            }

            if (eeprom_stat->cmd&0x01)
            {
                //Enter sequential read mode
                eeprom_stat->seq_ind= EEPROM_SEQ_SREAD_BYTES;
                //Do the first byte read
                eeprom_stat->bits_out= eeprom_raw_read(eeprom_stat->addr++);
                eeprom_stat->bits_out_index= 8;
                //eeprom_stat->bits_out >>= 1;
                //MinxRegs[VREG_IO_DATA] |= ((eeprom_receive_bit(1)&1)<<2);
                break;
            }
            else
                eeprom_stat->seq_ind= EEPROM_SEQ_ADDR_HI;
            break;
        case EEPROM_SEQ_ADDR_HI:
            //Enter high byte
            eeprom_stat->addr= (eeprom_stat->addr&0x00FF)|(data<<8);
            eeprom_stat->seq_ind++;
            break;
        case EEPROM_SEQ_ADDR_LOW:
            //Enter low byte
            eeprom_stat->addr= (eeprom_stat->addr&0xFF00)|data;
            eeprom_stat->seq_ind++;
            break;
        case EEPROM_SEQ_WR_BYTE:
            // Enter write payload byte
            if (!eeprom_stat->write_protect)
            {
                eeprom_raw_write(eeprom_stat->addr++, data);
            }
            break;
        case EEPROM_SEQ_SREAD_BYTES: //Sequential read
            break;
        default:
            eeprom_stat->seq_ind= EEPROM_SEQ_IDLE;
            return;
    }
}

void eeprom_send_bit(uint8_t bit, uint8_t clock)
{
    uint8_t rising_bit= (bit&1) & (~MinxRegs[VREG_IO_DATA_OLD]>>2);
    uint8_t falling_bit= ((~bit)&1) & (MinxRegs[VREG_IO_DATA_OLD]>>2);

    //Start command
    if (falling_bit && clock)
    {
        eeprom_stat->bits_in_index= 0;
        eeprom_stat->seq_ind= EEPROM_SEQ_CMD;
        return;
    }
    //Stop command
    if (rising_bit && clock)
    {
        eeprom_stat->clock= 0;
        eeprom_stat->bits_in_index= 0;
        eeprom_stat->seq_ind= EEPROM_SEQ_IDLE;
        return;
    }

    if (clock)
    switch (eeprom_stat->seq_ind)
    {
        case EEPROM_SEQ_IDLE:
            //Wait for start bit
            eeprom_stat->bits_in_index= 0;
            return;
        case EEPROM_SEQ_CMD:
        case EEPROM_SEQ_ADDR_HI:
        case EEPROM_SEQ_ADDR_LOW:
        case EEPROM_SEQ_WR_BYTE:
            eeprom_stat->bits_in= (eeprom_stat->bits_in<<1)|(bit&1);
            eeprom_stat->bits_in_index++;
            if (eeprom_stat->bits_in_index >= 8)
            {
                eeprom_stat->bits_in_index= 0;
                eeprom_send_byte(eeprom_stat->bits_in);
            }
            return;
        case EEPROM_SEQ_SREAD_BYTES:
            //Sequential read mode, stop if the stop bit is received
            eeprom_stat->bits_out= eeprom_raw_read(eeprom_stat->addr++);
            eeprom_stat->bits_out_index= 7;
            return;
        default:
            eeprom_stat->bits_in_index= 0;
            eeprom_stat->seq_ind= EEPROM_SEQ_IDLE;
            //eeprom_stat->clock= 0;
            return;
    }
}

uint8_t eeprom_receive_bit(uint8_t clock)
{
    if (!clock)
        return 0;

    if (eeprom_stat->bits_out_index >= 8)
    {
        eeprom_stat->bits_out_index--;
        return 0;
    }

    if (eeprom_stat->bits_out_index > 0)
        eeprom_stat->bits_out_index--;
    else
    {
        if (eeprom_stat->seq_ind == EEPROM_SEQ_SREAD_BYTES)
            eeprom_send_byte(0);
    }

    uint8_t result= (eeprom_stat->bits_out>>7)&1;
    eeprom_stat->bits_out= eeprom_stat->bits_out<<1;

    return result;
}

void lcd_send_cmd(uint8_t cmd)
{
    //Commands that don't need an additional data byte
    if (lcd_cmd == 0x00)
    switch (cmd)
    {
        case 0xA6:
        case 0xA7: //Display invert
            lcd_invert= cmd&1;
            prc_build_palette();
            break;
        //Handle cases for commands that need an additional data byte//
        case 0x81:
            lcd_cmd= cmd;
            return;
        //                                                           //
        default:
            return;
    }

    switch (lcd_cmd)
    {
        case 0x81: //Contrast adjust
            lcd_contrast= cmd&0x3F;
            prc_build_palette();
            break;
        default:
            break;
    }

    lcd_cmd= 0x00;
}
