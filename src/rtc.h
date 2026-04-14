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

#ifndef RTC_H
#define RTC_H

#define CARTIO_DATA     *((u16*)0x080000C4)
#define CARTIO_DIR      *((u16*)0x080000C6)
#define CARTIO_CTRL     *((u16*)0x080000C8)

typedef struct
{
    u8 year;
    u8 month;
    u8 day;
    u8 day_of_week;
    u8 hour;
    u8 minute;
    u8 second;
} datetime_t;

datetime_t get_datetime();

void set_timestamp_rtc();

#endif
