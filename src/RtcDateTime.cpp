/*-------------------------------------------------------------------------
RTC library

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/Rtc)

-------------------------------------------------------------------------
This file is part of the Makuna/Rtc library.

Rtc is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Rtc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Rtc.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#include <Arduino.h>
#include "RtcUtility.h"
#include "RtcDateTime.h"

const uint8_t c_daysInMonth[] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };


template <typename T> T DaysSinceFirstOfYear2000(uint16_t year, uint8_t month, uint8_t dayOfMonth)
{
    T days = dayOfMonth;
    for (uint8_t indexMonth = 1; indexMonth < month; ++indexMonth)
    {
        days += pgm_read_byte(c_daysInMonth + indexMonth - 1);
    }
    if (month > 2 && year % 4 == 0)
    {
        days++;
    }
    return days + 365 * year + (year + 3) / 4 - 1;
}

template <typename T> T SecondsIn(T days, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}

bool RtcDateTime::IsValid() const
{
    // this just tests the most basic validity of the value ranges
    // and valid leap years
    // It does not check any time zone or daylight savings time
    if ((_month > 0 && _month < 13) &&
        (_dayOfMonth > 0 && _dayOfMonth < 32) &&
        (_hour < 24) &&
        (_minute < 60) &&
        (_second < 60))
    {
        // days in a month tests
        //
        if (_month == 2)
        {
            if (_dayOfMonth > 29)
            {
                return false;
            }
            else if (_dayOfMonth > 28)
            {
                // leap day
                // check year to make sure its a leap year
                uint16_t year = Year();

                if ((year % 4) != 0)
                {
                    return false;
                }

                if ((year % 100) == 0 &&
                    (year % 400) != 0)
                {
                    return false;
                }
            }
        }
        else if (_dayOfMonth == 31)
        {
            if ((((_month - 1) % 7) % 2) == 1)
            {
                return false;
            }
        }

        return true;
    }
    return false;
}

uint8_t RtcDateTime::DayOfWeek() const
{
    uint16_t days = DaysSinceFirstOfYear2000<uint16_t>(_yearFrom2000, _month, _dayOfMonth);
    return (days + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

// 32-bit time; as seconds since 1/1/2000
uint32_t RtcDateTime::TotalSeconds() const
{
	uint16_t days = DaysSinceFirstOfYear2000<uint16_t>(_yearFrom2000, _month, _dayOfMonth);
	return SecondsIn<uint32_t>(days, _hour, _minute, _second);
}

// 64-bit time; as seconds since 1/1/2000
uint64_t RtcDateTime::TotalSeconds64() const
{
	uint32_t days = DaysSinceFirstOfYear2000<uint32_t>(_yearFrom2000, _month, _dayOfMonth);
	return SecondsIn<uint64_t>(days, _hour, _minute, _second);
}

// total days since 1/1/2000
uint16_t RtcDateTime::TotalDays() const
{
	return DaysSinceFirstOfYear2000<uint16_t>(_yearFrom2000, _month, _dayOfMonth);
}

// return the next day that falls on the given day of week
// if this day is that day of week, it will return this day
RtcDateTime RtcDateTime::NextDayOfWeek(uint8_t dayOfWeek) const
{
    uint8_t currentDayOfWeek = DayOfWeek();
    RtcDateTime result;
    int8_t delta = (dayOfWeek - currentDayOfWeek);
    
    // only want time in the future
    if (delta < 0)
    {
        delta += 7;
    }

    result = *this + (delta * c_DayAsSeconds);
    return result;
}

