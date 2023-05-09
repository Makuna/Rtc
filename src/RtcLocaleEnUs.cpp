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
#include "RtcTimeZone.h"
#include "RtcLocaleEnUs.h"

// due to how PROGMEM works, strings must be declared standalone
//
const char stringADT[] PROGMEM = "ADT";
const char stringAKDT[] PROGMEM = "AKDT";
const char stringAKST[] PROGMEM = "AKST";
const char stringAST[] PROGMEM = "AST";
const char stringCDT[] PROGMEM = "CDT";
const char stringCST[] PROGMEM = "CST";
const char stringEDT[] PROGMEM = "EDT";
const char stringEST[] PROGMEM = "EST";
const char stringGMT[] PROGMEM = "GMT";
const char stringHDT[] PROGMEM = "HDT";
const char stringHST[] PROGMEM = "HST";
const char stringMDT[] PROGMEM = "MDT";
const char stringMST[] PROGMEM = "MST";
const char stringPDT[] PROGMEM = "PDT";
const char stringPST[] PROGMEM = "PST";
const char stringUTC[] PROGMEM = "UTC";

// collection in alphabetic order of abbreviation
//
const RtcTimeZone c_tzEnUs[] PROGMEM =
		{
			{ stringADT, -3 * 60 },  // Atlantic Daylight Time
			{ stringAKDT, -8 * 60 }, // Alaska Daylight Time
			{ stringAKST, -9 * 60 }, // Alaska Standard Time
            { stringAST, -4 * 60 },  // Atlantic Standard Time
			{ stringCDT, -5 * 60 },  // Central Daylight Time
			{ stringCST, -6 * 60 },  // Central Standard Time
			{ stringEDT, -4 * 60 },  // Eastern Daylight Time
			{ stringEST, -5 * 60 },  // Eastern Standard Time
            { stringGMT, 0 },        // Greenwich Mean Time
			{ stringHDT, -9 * 60 },  // Hawaii–Aleutian Daylight Time
			{ stringHST, -10 * 60 }, // Hawaii–Aleutian Standard Time
			{ stringMDT, -6 * 60 },  // Mountain Daylight Time
			{ stringMST, -7 * 60 },  // Mountain Standard Time
			{ stringPDT, -7 * 60 },  // Pacific Daylight Time
			{ stringPST, -8 * 60 },  // Pacific Standard Time
			{ stringUTC, 0 },        // Cordinated Universal Time
		};

uint8_t RtcLocaleEnUs::CharsToMonth(const char* monthChars, size_t count)
{
    uint8_t month = 0;

    // works for both standard abbreviations and full month names
    //
    if (count >= 3)
    {
        switch (tolower(monthChars[0]))
        {
        case 'j':
            if (tolower(monthChars[1]) == 'a')
                month = 1;
            else if (tolower(monthChars[2]) == 'n')
                month = 6;
            else
                month = 7;
            break;
        case 'f':
            month = 2;
            break;
        case 'a':
            month = tolower(monthChars[1]) == 'p' ? 4 : 8;
            break;
        case 'm':
            month = tolower(monthChars[2]) == 'r' ? 3 : 5;
            break;
        case 's':
            month = 9;
            break;
        case 'o':
            month = 10;
            break;
        case 'n':
            month = 11;
            break;
        case 'd':
            month = 12;
            break;
        }
    }
    return month;
}

size_t RtcLocaleEnUs::TimeZoneMinutesFromAbbreviation(int32_t* minutes, 
    const char* abbr)
{
    RtcTimeZone entry;

    *minutes = 0;

    size_t result = RtcTimeZone::BinarySearchProgmemTable(&entry,
        abbr,
        c_tzEnUs,
        countof(c_tzEnUs));

    if (result)
    {
        // found
        *minutes = entry.offset;
    }

    return result;
}