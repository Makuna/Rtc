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

// ESP32 complains if not included
#if defined(ARDUINO_ARCH_ESP32)
#include <inttypes.h>
#endif

#include "RtcTimeZone.h"
#include "RtcLocaleEnUs.h"
#include "RtcLocaleEn.h"

enum DayOfWeek
{
    DayOfWeek_Sunday = 0,
    DayOfWeek_Monday,
    DayOfWeek_Tuesday,
    DayOfWeek_Wednesday,
    DayOfWeek_Thursday,
    DayOfWeek_Friday,
    DayOfWeek_Saturday,
};

const uint16_t c_OriginYear = 2000;
const uint32_t c_UnixEpoch32 = 946684800; // Unix origin year is 1970
const uint32_t c_NtpEpoch32FromUnixEpoch32 = 2208988800; // Ntp origin year is 1900
const uint32_t c_NtpEpoch32 = c_UnixEpoch32 + c_NtpEpoch32FromUnixEpoch32;

extern const uint8_t c_daysInMonth[] PROGMEM;

const uint32_t c_MinuteAsSeconds = 60;
const uint32_t c_HourAsSeconds = 60 * c_MinuteAsSeconds;
const uint32_t c_DayAsSeconds = 24 * c_HourAsSeconds;
const uint32_t c_WeekAsSeconds = 7 * c_DayAsSeconds;


// AM PM
enum RtcMeridiem
{
    Rtc_AM,
    Rtc_PM
};

// handy conversion class between 24 hour and 12 hour values
// 
class RtcHourAmPm
{
public:
    RtcHourAmPm() :
        _hour(12),
        _meridiem(Rtc_AM)
    {
    }

    // construct from a 24 hour units with validation
    // if outside bounds, wrap into bounds
    //
    RtcHourAmPm(uint8_t hour24)
    {
        if (hour24 < 1 || hour24 > 23)
        {
            // midnight is 12am
            _hour = 12;
            _meridiem = Rtc_AM;
        }
        else if (hour24 < 12)
        {
            _hour = hour24;
            _meridiem = Rtc_AM;
        }
        else if (hour24 > 12)
        {
            _hour = hour24 - 12;
            _meridiem = Rtc_PM;
        }
        else
        {
            // noon is 12pm
            _hour = 12;
            _meridiem = Rtc_PM;
        }
    }

    // construct from 12 hour units and meridiem with validation
    //
    RtcHourAmPm(uint8_t hour12, RtcMeridiem meridiem)
    {
        // if outside bounds, wrap into bounds
        if (hour12 < 1)
        {
            hour12 = 12;
        }
        else if (hour12 > 12)
        {
            hour12 = 1;
            meridiem = (meridiem == Rtc_AM) ? Rtc_PM : Rtc_AM;
        }

        _hour = hour12;
        _meridiem = meridiem;
    }

    // covert to 24 hour units
    //
    operator uint8_t() const
    {
        uint8_t result = _hour;

        if (result == 12)
        {
            if (_meridiem == Rtc_AM)
            {
                result = 0;
            }
        }
        else if (_meridiem == Rtc_PM)
        {
            result += 12;
        }

        return result;
    }

    // properties
    //
    uint8_t Hour() const
    {
        return _hour;
    }

    RtcMeridiem Meridiem() const
    {
        return _meridiem;
    }

    uint8_t Hour24() const
    {
        return *this;
    }

protected:
    uint8_t _hour;
    RtcMeridiem _meridiem;
};


class RtcDateTime
{
public:
    explicit RtcDateTime(uint32_t secondsFrom2000 = 0) 
    {
        _initWithSecondsFrom2000<uint32_t>(secondsFrom2000);
    }

    RtcDateTime(uint16_t year,
        uint8_t month,
        uint8_t dayOfMonth,
        uint8_t hour,
        uint8_t minute,
        uint8_t second) :
        _yearFrom2000((year >= c_OriginYear) ? year - c_OriginYear : year),
        _month(month),
        _dayOfMonth(dayOfMonth),
        _hour(hour),
        _minute(minute),
        _second(second)
    {
    }

    // RtcDateTime compileDateTime(__DATE__, __TIME__);
    // sample input: date = "Dec 06 2009", time = "12:34:56"
    RtcDateTime(const char* date, const char* time) :
        RtcDateTime(0)
    {
        // __DATE__ is always in english
        InitWithDateTimeFormatString<RtcLocaleEnUs>(F("MMM DD YYYY"), date);
        InitWithDateTimeFormatString<RtcLocaleEnUs>(F("hh:mm:ss"), time);
    }

    bool IsValid() const;

    uint16_t Year() const
    {
        return c_OriginYear + _yearFrom2000;
    }

    uint8_t Month() const
    {
        return _month;
    }

    uint8_t Day() const
    {
        return _dayOfMonth;
    }

    uint8_t Hour() const
    {
        return _hour;
    }

    RtcHourAmPm HourAmPm() const
    {
        return RtcHourAmPm(_hour);
    }

    uint8_t Minute() const
    {
        return _minute;
    }

    uint8_t Second() const
    {
        return _second;
    }

    // 0 = Sunday, 1 = Monday, ... 6 = Saturday
    uint8_t DayOfWeek() const;

    // 32-bit time; as seconds since 1/1/2000
	uint32_t TotalSeconds() const;

	// 64-bit time; as seconds since 1/1/2000
	uint64_t TotalSeconds64() const;

	// total days since 1/1/2000
	uint16_t TotalDays() const;
	
    // return the next day that falls on the given day of week
    // if this day is that day of week, it will return this day
    RtcDateTime NextDayOfWeek(uint8_t dayOfWeek) const;

    /* 
    causes ambiguous overload for 'operator+'
    making explicit doesn't solve it
    // add unsigned seconds
    void operator += (uint32_t seconds)
    {
        *this = *this + seconds;
    }
    
    RtcDateTime operator + (uint32_t seconds) const
    {
        return RtcDateTime(TotalSeconds() + seconds);
    }
    */

    // add signed seconds
    void operator += (int32_t seconds)
    {
        *this = *this + seconds;
    }

    RtcDateTime operator + (int32_t seconds) const
    {
        uint32_t totalSeconds = TotalSeconds();
        // never allowed to go before year 2000
        if (seconds < 0 && abs(seconds) > totalSeconds)
        {
            totalSeconds = 0;
        }
        else
        {
            totalSeconds += seconds;
        }
        return RtcDateTime(totalSeconds);
    }

    // remove seconds
    void operator -= (uint32_t seconds)
    {
        *this = *this - seconds;
    }

    RtcDateTime operator - (uint32_t seconds) const
    {
        return RtcDateTime(TotalSeconds() - seconds);
    }

    bool operator == (const RtcDateTime& right)
    {
        return (this->TotalSeconds() == right.TotalSeconds());
    }

    bool operator != (const RtcDateTime& right)
    {
        return (this->TotalSeconds() != right.TotalSeconds());
    }

    bool operator <= (const RtcDateTime& right)
    {
        return (this->TotalSeconds() <= right.TotalSeconds());
    }

    bool operator >= (const RtcDateTime& right)
    {
        return (this->TotalSeconds() >= right.TotalSeconds());
    }

    bool operator < (const RtcDateTime& right)
    {
        return (this->TotalSeconds() < right.TotalSeconds());
    }

    bool operator > (const RtcDateTime& right)
    {
        return (this->TotalSeconds() > right.TotalSeconds());
    }

    // Epoch32 support
    [[deprecated("Use Unix32Time() instead.")]]
    uint32_t Epoch32Time() const
    {
        return TotalSeconds() + c_UnixEpoch32;
    }
    [[deprecated("Use InitWithUnix32Time() instead.")]]
    void InitWithEpoch32Time(uint32_t secondsSince1970)
    {
        _initWithSecondsFrom2000<uint32_t>(secondsSince1970 - c_UnixEpoch32);
    }

    // Epoch64 support
    [[deprecated("Use Unix64Time() instead.")]]
    uint64_t Epoch64Time() const
    {
        return TotalSeconds64() + c_UnixEpoch32;
    }
    [[deprecated("Use InitWithUnix64Time() instead.")]]
    void InitWithEpoch64Time(uint64_t secondsSince1970)
    {
        _initWithSecondsFrom2000<uint64_t>(secondsSince1970 - c_UnixEpoch32);
    }

    // Unix32 support
    uint32_t Unix32Time() const
    {
        return TotalSeconds() + c_UnixEpoch32;
    }
    void InitWithUnix32Time(uint32_t secondsSince1970)
    {
        _initWithSecondsFrom2000<uint32_t>(secondsSince1970 - c_UnixEpoch32);
    }
    // Unix64 support
    uint64_t Unix64Time() const
    {
        return TotalSeconds64() + c_UnixEpoch32;
    }
    void InitWithUnix64Time(uint64_t secondsSince1970)
    {
        _initWithSecondsFrom2000<uint64_t>(secondsSince1970 - c_UnixEpoch32);
    }

    // Ntp32 support
    uint32_t Ntp32Time() const
    {
        return TotalSeconds() + c_NtpEpoch32;
    }
    void InitWithNtp32Time(uint32_t secondsSince1900)
    {
        _initWithSecondsFrom2000<uint32_t>(secondsSince1900 - c_NtpEpoch32);
    }
    // Ntp64 support
    uint64_t Ntp64Time() const
    {
        return TotalSeconds64() + c_NtpEpoch32;
    }
    void InitWithNtp64Time(uint64_t secondsSince1900)
    {
        _initWithSecondsFrom2000<uint64_t>(secondsSince1900 - c_NtpEpoch32);
    }

    [[deprecated("Use InitWithDateTimeFormatString()")]]
    void InitWithIso8601(const char* date)
    {
        // sample input: date = "Sat, 06 Dec 2009 12:34:56 GMT"
        InitWithDateTimeFormatString<RtcLocaleEnUs>(F("*, DD MMM YYYY hh:mm:ss zzz"), date);
    }

    //
    // https://www.w3.org/TR/NOTE-datetime
    // https://learn.microsoft.com/en-us/dotnet/standard/base-types/standard-date-and-time-format-strings
    // https://en.wikipedia.org/wiki/List_of_time_zone_abbreviations#:~:text=List%20of%20time%20zone%20abbreviations%20%20%20,%20%20UTC%2B08%3A00%20%2051%20more%20rows%20
    // 
    // * - ignore until next char
    // ! = ignore until not next char
    // 
    // YY - two digit year, assumes 2000 +
    // YYYY - four digit year
    // 
    // M - full month name, arbitrary length until next char
    // MM - two digit month
    // MMM - abreviated month name, 3 chars
    // 
    // DD - two digit day of month
    // 
    // hh - hour
    // mm - minute
    // ss - seconds
    // sssss - seconds with decimal (12.34)
    // 
    // z - +hh:mm or Z - 
    //      using this will adjust the time to UTC from the time zone
    //      using this will adjust the time to UTC from the time zone
    //      present in the string,
    //      without it, it will ignore the timezone and return the local
    // zzz - time zone abbreviation
    //      using this will adjust the time to UTC from the time zone
    //      present in the string,
    //      without it, it will ignore the timezone and return the local
    //
    // return - index last converted of datetime
    template <typename T_LOCALE = RtcLocaleEnUs> size_t InitWithDateTimeFormatString(
            const char* format, 
            const char* datetime)
    {
        const char specifiers[] = "*!YMDhmsz";
        const char* scan = format;
        const char* convert = datetime;
        int32_t timezoneMinutes = 0;

        // while chars in format and datetime
        while (*scan != '\0' && *datetime != '\0')
        {
            // find next token
            size_t iStart = strcspn(scan, specifiers);
            scan += iStart;
            convert += iStart;

            if (*scan != '\0')
            {
                // find the end of the token
                size_t iEnd = 1;
                while (scan[iEnd] == *scan)
                {
                    iEnd++;
                }
                size_t count = iEnd;
                size_t countConverted = 0;

                // handy debug tracing 
                //
                //Serial.print(scan[iStart]);
                //Serial.print(">");
                //Serial.print(convert);
                //Serial.print("< ");
                //Serial.print(count);
                //Serial.println();

                switch (scan[iStart])
                {
                case '*':
                    {
                        // increment through convert until the matching char 
                        // from scan after the * token
                        const char* skip = convert;
                        while (*skip != '\0' && *skip != scan[iEnd])
                        {
                            skip++;
                        }
                        // include skipping extra matching char
                        countConverted = skip - convert + 1;
                        count++;

                        // handy debug tracing 
                        //
                        //Serial.print("*>");
                        //Serial.print(scan + count);
                        //Serial.print("<->");
                        //Serial.print(convert + countConverted);
                        //Serial.print("< ");
                        //Serial.print(count);
                        //Serial.print("-");
                        //Serial.print(countConverted);
                        //Serial.println();
                    }
                    break;

                case '!':
                {
                    // increment through convert until the matching char 
                    // from scan after the * token is not present
                    const char* skip = convert;
                    while (*skip != '\0' && *skip == scan[iEnd])
                    {
                        skip++;
                    }
                    
                    countConverted = skip - convert;
                    count++;

                    // handy debug tracing 
                    //
                    //Serial.print("!>");
                    //Serial.print(scan + count);
                    //Serial.print("<->");
                    //Serial.print(convert + countConverted);
                    //Serial.print("< ");
                    //Serial.print(count);
                    //Serial.print("-");
                    //Serial.print(countConverted);
                    //Serial.println();
                }
                break;

                case 'Y':
                    if (count >= 4)
                    {
                        // only care about last three digits
                        size_t offset = count - 3;
                        scan += offset;
                        convert += offset;
                        count = 3;
                    }
                    countConverted = CharsToNumber<uint8_t>(convert, &_yearFrom2000, count);
                    break;

                case 'M':
                    if (*convert >= '0' && *convert <= '9')
                    {
                        if (count > 2)
                        {
                            return convert - datetime;
                        }
                        countConverted = CharsToNumber<uint8_t>(convert, &_month, count);
                    }
                    else
                    {
                        if (count > 3)
                        {
                            return convert - datetime;
                        }
                        else if (count == 1)
                        {
                            const char* temp = convert;
                            // increment temp until matching char after M
                            while (*temp != scan[iEnd])
                            {
                                temp++;
                            }

                            size_t monthCount = temp - convert;
                            if (monthCount < 3)
                            {
                                return convert - datetime;
                            }

                            _month = T_LOCALE::CharsToMonth(convert, monthCount);
                            countConverted = monthCount;
                        }
                        else 
                        {
                            _month = T_LOCALE::CharsToMonth(convert, count);
                            countConverted = count;
                        }
                    }
                    break;

                case 'D':
                    countConverted = CharsToNumber<uint8_t>(convert, &_dayOfMonth, count);
                    break;

                case 'h':
                    countConverted = CharsToNumber<uint8_t>(convert, &_hour, count);
                    break;

                case 'm':
                    countConverted = CharsToNumber<uint8_t>(convert, &_minute, count);
                    break;

                case 's':
                    countConverted = CharsToNumber<uint8_t>(convert, &_second, count);
                    break;

                case 'z':
                    if (count == 1)
                    {
                        const char* temp = convert;

                        // +hh:mm or Z formated timezone
                        // adjusting to local time
                        if (*temp == '+' || *temp == '-')
                        {
                            uint8_t hours;
                            uint8_t minutes;

                            int32_t timezoneSign = (*temp == '+') ? 1 : -1;
                            temp++;
                            temp += CharsToNumber<uint8_t>(temp, &hours, 2);
                            temp++; // :
                            temp += CharsToNumber<uint8_t>(temp, &minutes, 2);
                            timezoneMinutes = (static_cast<int32_t>(hours) * 60 + minutes) * timezoneSign;

                            countConverted = temp - datetime;
                        }
                        else if (*temp == 'Z' || *temp == 'z')
                        {
                            // nothing to adjust, 
                            // zulu time is what we want
                            countConverted = 1;
                        }
                        else
                        {
                            return convert - datetime;
                        }
                    }
                    else
                    {
                        // zzz - abbreviation timezone format
                        // adjust from local time
                        countConverted = T_LOCALE::TimeZoneMinutesFromAbbreviation(&timezoneMinutes, convert);
                    }
                    break;
                }

                if (countConverted)
                {
                    scan += count;
                    convert += countConverted;
                }
                else
                {
                    return convert - datetime;
                }
            }
        }

        // adjust our time by the timezone to get GMT/Z
        // as by default RtcDateTime is GMT
        //
        *this -= timezoneMinutes * 60;

        return convert - datetime;
    }

    // Version of above but supporting PROGMEM for the format, 
    // specifically the F("") use for format
    template <typename T_LOCALE = RtcLocaleEnUs> size_t InitWithDateTimeFormatString(
        const __FlashStringHelper* format,
        const char* datetime)
    {
        char ramFormat[32];
        
        strncpy_P(ramFormat, 
            reinterpret_cast<const char*>(format), 
            countof(ramFormat));
        return InitWithDateTimeFormatString<T_LOCALE>(ramFormat, datetime);
    }
    
    // convert our Day of Week to Rtc Day of Week 
    // RTC Hardware Day of Week is 1-7, 1 = Monday
    static uint8_t ConvertDowToRtc(uint8_t dow)
    {
        if (dow == 0)
        {
            dow = 7;
        }
        return dow;
    }

    // convert Rtc Day of Week to our Day of Week
    static uint8_t ConvertRtcToDow(uint8_t rtcDow)
    {
        return (rtcDow % 7);
    }

    // returns the number days in the month
    // month (1-12)
    static uint8_t DaysInMonth(uint16_t year, uint8_t month)
    {
        uint8_t zMonth = 0;
        // cap and convert to zero based
        if (month != 0)
        {
            if (month > 11)
            {
                zMonth = 11;
            }
            else
            {
                zMonth = month - 1;
            }
        }

        uint8_t days = pgm_read_byte(c_daysInMonth + zMonth);
        // check february for leap years
        if (month == 2 && IsLeapYear(year))
        {
            days++;
        }
        return days;
    }

    static bool IsLeapYear(uint16_t year)
    {
        return ((year % 4) == 0);
    }

protected:
    uint8_t _yearFrom2000;
    uint8_t _month;
    uint8_t _dayOfMonth;
    uint8_t _hour;
    uint8_t _minute;
    uint8_t _second;

    template <typename T> void _initWithSecondsFrom2000(T secondsFrom2000)
    {
        _second = secondsFrom2000 % 60;
        T timeFrom2000 = secondsFrom2000 / 60;
        _minute = timeFrom2000 % 60;
        timeFrom2000 /= 60;
        _hour = timeFrom2000 % 24;
        T days = timeFrom2000 / 24;
        T leapDays;

        for (_yearFrom2000 = 0;; ++_yearFrom2000)
        {
            leapDays = (_yearFrom2000 % 4 == 0) ? 1 : 0;
            if (days < 365U + leapDays)
                break;
            days -= 365 + leapDays;
        }
        for (_month = 1;; ++_month)
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

    // CharsToNumber - convert a series of chars to a number of the given type
    // 
    // str - the pointer to string to process
    // result - the value converted from the string
    // count - the number of numerals to stop processing with 
    //         excludes leading non-numeral chars
    // return - the number to increment str for more processing,
    //          0 for failure
    template <typename T_NUMBER> size_t CharsToNumber(const char* str, T_NUMBER* result, size_t count)
    {
        const char* scan = str;
        bool converted = false;
        size_t left = count;

        *result = 0;

        // skip leading 0 and non numericals
        while (left && '\0' != *scan && ('0' >= *scan || '9' < *scan))
        {
            // only decrement left with numerals
            if (left && '0' == *scan)
            {
                converted = true;
                left--;
            }
            scan++;
        }

        // calculate number until we hit non-numeral char
        while (left && '\0' != *scan && '0' <= *scan && *scan <= '9')
        {
            converted = true;

            *result *= 10;
            *result += *scan - '0';

            left--;
            scan++;
        }

        // we ignore decimal numbers but we need to scan past them
        //
        if (left && '.' == *scan)
        {
            // skip decimal
            left--;
            scan++;
            
            // continue to discard numerals
            while (left && '\0' != *scan && '0' <= *scan && *scan <= '9')
            {
                left--;
                scan++;
            }
        }

        return converted ? scan - str : 0;
    }
};


