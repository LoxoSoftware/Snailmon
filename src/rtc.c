/*
 *  Snailmon - Pokémon Mini emulator for the GBA
 *  Copyright (C) 2026  LoxoSoftware
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "include.h"

extern uint8_t option_cart_rtc;

datetime_t get_datetime()
{
    void delay(int amount)
    {
        for (int i=0; i<amount; i++)
            asm volatile ("nop");
    }

    void rtc_send_bit(uint8_t bit)
    {
        CARTIO_DATA= (1<<2)|((bit&1)<<1)|0;
        delay(8);
        CARTIO_DATA= (1<<2)|((bit&1)<<1)|1;
        delay(8);
    }

    bool rtc_get_bit()
    {
        CARTIO_DATA= (1<<2)|0;
        delay(8);
        CARTIO_DATA= (1<<2)|1;
        delay(8);
        return (CARTIO_DATA>>1)&1;
    }

    void rtc_send_byte(uint8_t data)
    {
        CARTIO_DIR= 0b111;
        delay(8);
        CARTIO_DATA= 0b011;
        delay(8);
        CARTIO_DATA= 0b111;
        delay(8);

        for (int ib=0; ib<8; ib++)
        {
            rtc_send_bit((data>>7)&1);
            data <<= 1;
        }

        delay(8);
    }

    uint8_t rtc_read_byte()
    {
        uint8_t buf= 0x00;

        CARTIO_DIR= 0b101;

        for (int ib=0; ib<8; ib++)
        {
            buf >>= 1;
            buf |= (rtc_get_bit()<<7);
        }

        delay(8);

        return buf;
    }

    uint8_t bcd2byte(uint8_t data)
    {
        uint8_t msd= (data>>4)&0x0F;
        uint8_t lsd= (data)&0x0F;

        return (10*msd)+lsd;
    }

    datetime_t ttime;

    CARTIO_CTRL= 0b1;
    delay(8);
    CARTIO_DATA= 0b011;
    delay(8);

    //Enter datetime command (0b01100101)
    rtc_send_byte(0x65);

    ttime.year= bcd2byte(rtc_read_byte());
    ttime.month= bcd2byte(rtc_read_byte());
    ttime.day= bcd2byte(rtc_read_byte());
    ttime.day_of_week= bcd2byte(rtc_read_byte());
    ttime.hour= bcd2byte(rtc_read_byte()&0x7F);
    ttime.minute= bcd2byte(rtc_read_byte());
    ttime.second= bcd2byte(rtc_read_byte()&0x7F);

    CARTIO_CTRL= 0b1;

    return ttime;
}

void set_timestamp_rtc()
{
    datetime_t t= get_datetime();

    if (t.year == 0 || t.month == 0 || t.day == 0 || t.day_of_week > 7)
    {
        //RTC is invalid
        MinxRegs[VREG_SYS_CTRL3] &= 0xFD; // Unset RTC valid bit
        option_cart_rtc= 0;
        return;
    }

    MinxRegs[VREG_SYS_CTRL3] |= 0x02; //Set RTC valid bit

    EEPROM[0x1FF6]= 0x00;
    EEPROM[0x1FF7]= 0x00;
    EEPROM[0x1FF8]= 0x00;
    EEPROM[0x1FF9]= t.year;
    EEPROM[0x1FFA]= t.month;
    EEPROM[0x1FFB]= t.day;
    EEPROM[0x1FFC]= t.hour;
    EEPROM[0x1FFD]= t.minute;
    EEPROM[0x1FFE]= t.second;
    EEPROM[0x1FFF]= (uint8_t)(t.year+t.month+t.day+t.hour+t.minute+t.second); //Checksum
}
