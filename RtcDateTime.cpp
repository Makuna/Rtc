
#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#include "RtcDateTime.h"
#include <Arduino.h>


static const uint8_t c_daysInMonth[] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

RtcDateTime::RtcDateTime(uint32_t secondsFrom2000)
{
    _second = secondsFrom2000 % 60;
    uint32_t timeFrom2000 = secondsFrom2000 / 60;
    _minute = timeFrom2000 % 60;
    timeFrom2000 /= 60;
    _hour = timeFrom2000 % 24;
    uint16_t days = timeFrom2000 / 24;
    uint8_t leapDays;

    for (_yearFrom2000 = 0; ; ++_yearFrom2000)
    {
        leapDays = (_yearFrom2000 % 4 == 0) ? 1 : 0;
        if (days < 365U + leapDays)
            break;
        days -= 365 + leapDays;
    }
    for (_month = 1; ; ++_month)
    {
        uint8_t daysPerMonth = pgm_read_byte(c_daysInMonth + _month - 1);
        if (leapDays && _month == 2)
            daysPerMonth++;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    _dayOfMonth = days + 1;
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
    // sample input: date = "Dec 26 2009", time = "12:34:56"
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

uint16_t DaysSinceFirstOfYear2000(uint16_t year, uint8_t month, uint8_t dayOfMonth)
{
    uint16_t days = dayOfMonth;
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

uint32_t SecondsIn(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}

uint8_t RtcDateTime::DayOfWeek() const
{
    uint16_t days = DaysSinceFirstOfYear2000(_yearFrom2000, _month, _dayOfMonth);
    return (days + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t RtcDateTime::TotalSeconds() const
{
    uint16_t days = DaysSinceFirstOfYear2000(_yearFrom2000, _month, _dayOfMonth);
    return SecondsIn(days, _hour, _minute, _second);
}