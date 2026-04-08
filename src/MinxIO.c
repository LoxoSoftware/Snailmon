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
        case 1:
            //Enter CMD
            eeprom_stat->cmd= data;
            eeprom_stat->seq_ind++;
            return;
        case 2:
            //Enter high byte
            if ((eeprom_stat->cmd&0xF0) != 0xA0) //Check control sequence
                break;
            eeprom_stat->addr_hi= data;
            eeprom_stat->seq_ind++;
            eeprom_stat->data_read_done= 0;
            return;
        case 3:
            //Enter low byte
            eeprom_stat->addr_lo= data;
            eeprom_stat->seq_ind++;
            return;
        default:
            eeprom_stat->seq_ind= 0;
            return;
    }

    eeprom_stat->seq_ind= 0;
}

void eeprom_send_bit(uint8_t bit)
{
    switch (eeprom_stat->seq_ind)
    {
        case 0:
            //Wait for start bit
            if (!bit)
                goto seq_over;
            eeprom_stat->seq_ind++;
            return;
        case 1:
        case 2:
        case 3:
            //Wait for an instruction byte
            eeprom_stat->bits_in= (eeprom_stat->bits_in<<1)|(bit&1);
            eeprom_stat->bits_in_index++;
            if (eeprom_stat->bits_in_index >= 8)
            {
                eeprom_stat->bits_in_index= 0;
                eeprom_send_byte(eeprom_stat->bits_in);
            }
            break;
        case 4:
            if (!(eeprom_stat->cmd&0x01)) //Read data
            {
                //Wait for stop bit and execute
                if (!bit)
                    return;
                    //goto seq_over;
                eeprom_stat->bits_out= eeprom_raw_read((eeprom_stat->addr_lo)|(eeprom_stat->addr_hi<<8));
                eeprom_stat->data_read_done= 1;
                goto seq_over;
            }
            else //Write data
            {
                //Wait for the data byte to write
                eeprom_stat->bits_in= (eeprom_stat->bits_in<<1)|(bit&1);
                eeprom_stat->bits_in_index++;
                if (eeprom_stat->bits_in_index >= 8)
                {
                    eeprom_stat->bits_in_index= 0;
                    eeprom_stat->seq_ind= 5;
                    return;
                }
            }
            return;
        case 5:
            //Wait for stop bit and execute (data write only)
            if (!bit)
                return;
                //goto seq_over;
            eeprom_raw_write((eeprom_stat->addr_lo)|(eeprom_stat->addr_hi<<8), eeprom_stat->bits_in);
            goto seq_over;
        default:
seq_over:
            eeprom_stat->seq_ind= 0;
            //eeprom_stat->clock= 0;
            return;
    }
}

uint8_t eeprom_receive_bit()
{
    uint8_t result= (eeprom_stat->bits_out>>7)&1;
    eeprom_stat->bits_out= eeprom_stat->bits_out<<1;
    return result;
}
