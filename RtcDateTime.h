

#ifndef __RTCDATETIME_H__
#define __RTCDATETIME_H__

const uint16_t c_OriginYear = 2000;
const uint32_t c_Epoch32OfOriginYear = 946684800;
extern const uint8_t c_daysInMonth[] PROGMEM;

class RtcDateTime
{
public:
    RtcDateTime(uint32_t secondsFrom2000 = 0);
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
    RtcDateTime(const char* date, const char* time);

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
    uint8_t DayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    uint32_t TotalSeconds() const;
    uint64_t TotalSeconds64() const;

    // add seconds
    void operator += (uint32_t seconds)
    {
        RtcDateTime after = RtcDateTime( TotalSeconds() + seconds );
        *this = after;
    }

    // allows for comparisons to just work (==, <, >, <=, >=, !=)
    operator uint32_t() const 
    { 
        return TotalSeconds(); 
    }

    // Epoch32 support
    uint32_t Epoch32Time() const
    {
        return TotalSeconds() + c_Epoch32OfOriginYear;
    }
    void InitWithEpoch32Time(uint32_t time)
    {
        _initWithSecondsFrom2000<uint32_t>(time - c_Epoch32OfOriginYear);
    }

    // Epoch64 support
    uint64_t Epoch64Time() const
    {
        return TotalSeconds64() + c_Epoch32OfOriginYear;
    }
    void InitWithEpoch64Time(uint64_t time)
    {
        _initWithSecondsFrom2000<uint64_t>(time - c_Epoch32OfOriginYear);
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

};

#endif // __RTCDATETIME_H__