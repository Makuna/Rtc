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

struct RtcTimeZone
{
    const char* abbr; // progmem
    int32_t offset;

	// searches the given table for the abbr match
	//  result - the found entry
	//  abbrSearch - the abbreviation to search for
	//  table - the PROGMEM table of RtcTimeZone in alphabetical order
	//  count - the number of entries in the table
	//  returns: 0 for not found, strlen of abbr if found
	//
    static size_t BinarySearchProgmemTable(RtcTimeZone* result,
        const char* abbrSearch,
        const RtcTimeZone* table,
        size_t countTable)
    {
		size_t indexFront = 0;
		size_t indexEnd = countTable;
		size_t index = countTable / 2;

		// binary search
		//
		while (indexFront < indexEnd)
		{
			RtcTimeZone entry;

			// copy the entry from progmem
			// string members still reside in PROGMEM though
			memcpy_P(&entry, table + index, sizeof(RtcTimeZone));

			auto match = strncmp_P(abbrSearch, entry.abbr, 4);

			if (0 == match)
			{
				// found
				*result = entry;
				return strlen_P(entry.abbr);
			}
			else if (match < 0)
			{
				indexEnd = index;
			}
			else
			{
				indexFront = index + 1;
			}

			// pick one between the front and end
			index = (indexEnd - indexFront) / 2 + indexFront;
		}

		return 0;
    }

	// searches the given table for the abbr match
	//  result - the found entry
	//  abbrSearch - the abbreviation to search for
	//  table - the PROGMEM table of RtcTimeZone to search
	//  count - the number of entries in the table
	//  returns: 0 for not found, strlen of abbr if found
	//
	static size_t LinearSearchProgmemTable(RtcTimeZone* result,
		const char* abbrSearch,
		const RtcTimeZone* table,
		size_t countTable)
	{
		for (size_t index = 0; index < countTable; index++)
		{
			RtcTimeZone entry;

			// copy the entry from progmem
			// string members still reside in PROGMEM though
			memcpy_P(&entry, table + index, sizeof(RtcTimeZone));

			if (0 == strncmp_P(abbrSearch, entry.abbr, 4))
			{
				*result = entry;
				return strlen_P(entry.abbr);
			}
		}

		return 0;
	}
};