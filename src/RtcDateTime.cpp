
#include <Arduino.h>
#include "RtcDateTime.h"

const uint8_t c_daysInMonth[] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

RtcDateTime::RtcDateTime(uint32_t secondsFrom2000)
{
    _initWithSecondsFrom2000<uint32_t>(secondsFrom2000);
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

uint8_t StringToUint8(const char* pString)
{
    uint8_t value = 0;

    // skip leading 0 and spaces
    while ('0' == *pString || *pString == ' ')
    {
        pString++;
    }

    // calculate number until we hit non-numeral char
    while ('0' <= *pString && *pString <= '9')
    {
        value *= 10;
        value += *pString - '0';
        pString++;
    }
    return value;
}

RtcDateTime::RtcDateTime(const char* date, const char* time)
{
    // sample input: date = "Dec 06 2009", time = "12:34:56"
    _yearFrom2000 = StringToUint8(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (date[0])
    {
    case 'J':
        if ( date[1] == 'a' )
            _month = 1;
        else if ( date[2] == 'n' )
            _month = 6;
        else
            _month = 7;
        break;
    case 'F':
        _month = 2;
        break;
    case 'A':
        _month = date[1] == 'p' ? 4 : 8;
        break;
    case 'M':
        _month = date[2] == 'r' ? 3 : 5;
        break;
    case 'S':
        _month = 9;
        break;
    case 'O':
        _month = 10;
        break;
    case 'N':
        _month = 11;
        break;
    case 'D':
        _month = 12;
        break;
    }
    _dayOfMonth = StringToUint8(date + 4);
    _hour = StringToUint8(time);
    _minute = StringToUint8(time + 3);
    _second = StringToUint8(time + 6);
}

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

void RtcDateTime::InitWithIso8601(const char* date)
{
    // sample input: date = "Sat, 06 Dec 2009 12:34:56 GMT"
    _yearFrom2000 = StringToUint8(date + 13);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (date[8])
    {
    case 'J':
        if (date[1 + 8] == 'a')
            _month = 1;
        else if (date[2 + 8] == 'n')
            _month = 6;
        else
            _month = 7;
        break;
    case 'F':
        _month = 2;
        break;
    case 'A':
        _month = date[1 + 8] == 'p' ? 4 : 8;
        break;
    case 'M':
        _month = date[2 + 8] == 'r' ? 3 : 5;
        break;
    case 'S':
        _month = 9;
        break;
    case 'O':
        _month = 10;
        break;
    case 'N':
        _month = 11;
        break;
    case 'D':
        _month = 12;
        break;
    }
    _dayOfMonth = StringToUint8(date + 5);
    _hour = StringToUint8(date + 17);
    _minute = StringToUint8(date + 20);
    _second = StringToUint8(date + 23);
}
