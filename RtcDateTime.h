

#ifndef __RTCDATETIME_H__
#define __RTCDATETIME_H__

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
            _yearFrom2000((year >= 2000) ? year - 2000 : year),
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
        return 2000 + _yearFrom2000;
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

protected:
    uint8_t _yearFrom2000;
    uint8_t _month;
    uint8_t _dayOfMonth;
    uint8_t _hour;
    uint8_t _minute;
    uint8_t _second;
};

#endif // __RTCDATETIME_H__