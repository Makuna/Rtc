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

#ifndef __RTCDATETIME_H__
#define __RTCDATETIME_H__

// ESP32 complains if not included
#if defined(ARDUINO_ARCH_ESP32)
#include <inttypes.h>
#endif

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

class RtcLocaleEN
{
public:
    static uint8_t CharsToMonth(const char* monthChars, size_t count)
    {
        uint8_t month = 0;

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
    RtcDateTime(const char* date, const char* time)
    {
        // __DATE__ is always in english
        InitWithDateTimeFormatString<RtcLocaleEN>("MMM DD YYYY", date);
        InitWithDateTimeFormatString<RtcLocaleEN>("hh:mm:ss", time);
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
	
    // add seconds
    void operator += (uint32_t seconds)
    {
        RtcDateTime after = RtcDateTime( TotalSeconds() + seconds );
        *this = after;
    }

    RtcDateTime operator + (uint32_t seconds) const
    {
        RtcDateTime after = RtcDateTime(TotalSeconds() + seconds);
        return after;
    }

    // remove seconds
    void operator -= (uint32_t seconds)
    {
        RtcDateTime before = RtcDateTime( TotalSeconds() - seconds );
        *this = before;
    }

    RtcDateTime operator - (uint32_t seconds) const
    {
        RtcDateTime after = RtcDateTime(TotalSeconds() - seconds);
        return after;
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
        InitWithDateTimeFormatString<RtcLocaleEN>("*, DD MMM YYYY hh:mm:ss zzz", date);
    }

    //
    // https://www.w3.org/TR/NOTE-datetime
    // https://learn.microsoft.com/en-us/dotnet/standard/base-types/standard-date-and-time-format-strings
    // 
    // * - ignore until next char
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
    // 
    // z -
    // zzz - time zone
    //
    template <typename T_LOCALE> bool InitWithDateTimeFormatString(const char* format, const char* datetime)
    {
        const char specifiers[] = "*YMDhmsz";
        const char* scan = format;
        const char* convert = datetime;

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


                switch (scan[iStart])
                {
                case '*':
                    // increment convert until matching char after *
                    while (*convert != scan[iEnd])
                    {
                        convert++;
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
                    _yearFrom2000 = CharsToNumber<uint8_t>(convert, count);
                    break;

                case 'M':
                    if (*convert >= '0' && *convert <= '9')
                    {
                        if (count > 2)
                        {
                            return false;
                        }
                        _month = CharsToNumber<uint8_t>(convert, count);
                    }
                    else
                    {
                        if (count > 3)
                        {
                            return false;
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
                                return false;
                            }
                            _month = T_LOCALE::CharsToMonth(convert, monthCount);
                            convert = temp;
                        }
                        else 
                        {
                            _month = T_LOCALE::CharsToMonth(convert, count);
                        }
                    }
                    break;

                case 'D':
                    _dayOfMonth = CharsToNumber<uint8_t>(convert, count);
                    break;

                case 'h':
                    _hour = CharsToNumber<uint8_t>(convert, count);
                    break;

                case 'm':
                    _minute = CharsToNumber<uint8_t>(convert, count);
                    break;

                case 's':
                    _second = CharsToNumber<uint8_t>(convert, count);
                    break;

                case 'z':
                    // TODO: implement time zone adjustments
                    // right now, assuming no offset
                    break;
                }

                scan += count;
                convert += count;
            }
        }
        return true;
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

    template <typename T_NUMBER> T_NUMBER CharsToNumber(const char* chars, size_t count)
    {
        T_NUMBER value = 0;

        // skip leading 0 and non numericals
        while (count && ('0' >= *chars || '9' < *chars))
        {
            count--;
            chars++;
        }

        // calculate number until we hit non-numeral char
        while (count && '0' <= *chars && *chars <= '9')
        {
            value *= 10;
            value += *chars - '0';

            count--;
            chars++;
        }
        return value;
    }
};

#endif // __RTCDATETIME_H__
