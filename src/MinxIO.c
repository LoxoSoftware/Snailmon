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
    eeprom_stat->seq_ind= 0;
    eeprom_stat->cmd= 0;
    eeprom_stat->addr= 0;
    eeprom_stat->ack= 0;
    eeprom_stat->write_protect= 1;
}

inline uint8_t eeprom_raw_read(uint16_t ofs)
{
    return EEPROM[ofs&0x1FFF];
}

inline void eeprom_raw_write(uint16_t ofs, uint8_t data)
{
    EEPROM[ofs&0x1FFF]= data;
}

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
            eeprom_stat->seq_ind++;
            //eeprom_stat->ack= 1;

            if (eeprom_stat->cmd&0x01)
            {
                //Enter sequential read mode
                eeprom_stat->seq_ind= EEPROM_SEQ_SREAD_BYTES;
                //Do the first byte read
                eeprom_stat->bits_out= eeprom_raw_read(eeprom_stat->addr++);
                eeprom_stat->ack= 1;
                break;
            }
            break;
        case EEPROM_SEQ_ADDR_HI:
            //Enter high byte
            eeprom_stat->addr= (eeprom_stat->addr&0x00FF)|(data<<8);
            eeprom_stat->seq_ind++;
            //eeprom_stat->ack= 1;
            break;
        case EEPROM_SEQ_ADDR_LOW:
            //Enter low byte
            eeprom_stat->addr= (eeprom_stat->addr&0xFF00)|data;
            eeprom_stat->seq_ind++;
            //eeprom_stat->ack= 1;
            break;
        case EEPROM_SEQ_WR_BYTE:
            // Enter write payload byte
            if (!eeprom_stat->write_protect)
            {
                eeprom_raw_write(eeprom_stat->addr, data);
            }
            eeprom_stat->seq_ind++;
            eeprom_stat->ack= 0;
            break;
        case EEPROM_SEQ_SREAD_BYTES: //Sequential read
            break;
        default:
            eeprom_stat->seq_ind= EEPROM_SEQ_IDLE;
            eeprom_stat->ack= 0;
            return;
    }
}

void eeprom_send_bit(uint8_t bit)
{
    eeprom_stat->ack= 0;

    switch (eeprom_stat->seq_ind)
    {
        case EEPROM_SEQ_IDLE:
            //Wait for start bit
            if (!bit)
                return;
            eeprom_stat->seq_ind++;
            return;
        case EEPROM_SEQ_CMD:
        case EEPROM_SEQ_ADDR_HI:
        case EEPROM_SEQ_ADDR_LOW:
        case EEPROM_SEQ_WR_BYTE:
            //Wait for an instruction byte or a start bit
            if (eeprom_stat->seq_ind == EEPROM_SEQ_WR_BYTE && eeprom_stat->write_protect)
            {
                //Terminate byte write command if write protection is on
                if (!bit)
                    return;
                eeprom_stat->bits_in_index= -1; //Discard ACK bit
                eeprom_stat->seq_ind= 1;
            }
            eeprom_stat->bits_in= (eeprom_stat->bits_in<<1)|(bit&1);
            eeprom_stat->bits_in_index++;
            if (eeprom_stat->bits_in_index >= 8)
            {
                eeprom_stat->bits_in_index= 0;
                eeprom_send_byte(eeprom_stat->bits_in);
            }
            return;
        case EEPROM_SEQ_WR_BYTE_AWAIT:
            //Wait for stop bit
            if (!bit)
                return;
            eeprom_raw_write(eeprom_stat->addr, eeprom_stat->bits_in);
            goto seq_over;
            return;
        case EEPROM_SEQ_SREAD_BYTES:
            //Sequential read mode, stop if the stop bit is received
            if (bit)
                goto seq_over;
            eeprom_stat->bits_out= eeprom_raw_read(eeprom_stat->addr++);
            return;
        default:
seq_over:
            eeprom_stat->bits_in_index= 0;
            eeprom_stat->seq_ind= EEPROM_SEQ_IDLE;
            //eeprom_stat->clock= 0;
            return;
    }
}

uint8_t eeprom_receive_bit()
{
    if (eeprom_stat->ack)
    {
        eeprom_stat->ack--;
        return 1;
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
