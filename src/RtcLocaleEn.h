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

#pragma once

class RtcLocaleEn
{
public:
    static uint8_t CharsToMonth(const char* monthChars, size_t count);

    // TimeZoneMinutesFromAbbreviation
    // searches known table for the abbreviation and provides the minutes offset
    //
    //   *minutes - [out] variable filled with the minutes offset 
    //   returns - the number of chars parsed, usually 3 or 4.  0 if not found 
    static size_t TimeZoneMinutesFromAbbreviation(int32_t* minutes, const char* abbr);
};

