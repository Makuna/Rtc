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

#include <Arduino.h>
#include "RtcUtility.h"
#include "RtcDateTime.h"

enum AlarmPeriod
{
    AlarmPeriod_Expired,
    AlarmPeriod_SingleFire,
    AlarmPeriod_Yearly, // Just use this one, not the next three
    AlarmPeriod_Yearly_Feb29th, // last of month in Feb if days less than and not a leap year, 
    AlarmPeriod_Monthly, // Just use this one, not the next three
    AlarmPeriod_Monthly_29th, // last of month if days less than, 
    AlarmPeriod_Monthly_30th, // otherwise the day of month matching,
    AlarmPeriod_Monthly_31st, // this will be set internally, just use monthly
    AlarmPeriod_Weekly, 
    AlarmPeriod_Daily,
    AlarmPeriod_Hourly,
};

typedef void(*RtcAlarmCallback)(uint8_t index);

template <RtcAlarmCallback V_CALLBACK> class RtcAlarmManager
{
public:
    RtcAlarmManager(uint8_t count) :
        _alarmsCount(count)
    {
        _alarms = new Alarm[_alarmsCount];
    }

    ~RtcAlarmManager()
    {
        delete[] _alarms;
    }

    int32_t Sync(const RtcDateTime& now)
    {
        uint32_t msNow = millis();
        uint32_t secondsNow = now.TotalSeconds();
        // calc an updated seconds for old information
        uint32_t secondsOld = _seconds + (msNow - _msLast) / 1000;
        // set new seconds and start tracking the millis
        _msLast = msNow;
        _seconds = secondsNow;
        // return the delta from new seconds from old seconds
        return (secondsNow - secondsOld);
    }

    RtcDateTime Now() const
    {
        uint32_t msNow = millis();
        uint32_t secondsNow = _seconds + (msNow - _msLast) / 1000;
        return RtcDateTime(secondsNow);
    }

    int8_t AddAlarm(const RtcDateTime& when,
        AlarmPeriod period)
    {
        if (when.IsValid())
        {
            uint32_t seconds = when.TotalSeconds();

            if (period == AlarmPeriod_Monthly)
            {
                // adjust alarm period to store target day of month
                // for when months have less days than the target
                // it will trigger on the last day of the month but
                // retain and trigger on specific day of month when
                // available
                if (when.Day() == 29)
                {
                    period = AlarmPeriod_Monthly_29th;
                }
                else if (when.Day() == 30)
                {
                    period = AlarmPeriod_Monthly_30th;
                }
                else if (when.Day() == 31)
                {
                    period = AlarmPeriod_Monthly_31st;
                }
            }
            else if (period == AlarmPeriod_Yearly)
            {
                if (when.Day() == 29 && when.Month() == 2)
                {
                    // adjust alarm period to store target day of month
                    // for when Feb 29th is target but following year isnt 
                    // a leap year it will trigger on the last day of Feb but
                    // retain and trigger on specific day of month when
                    // available
                    period = AlarmPeriod_Yearly_Feb29th;
                }
            }

            Alarm alarm = { seconds, period };

            if (seconds <= _seconds)
            {
                alarm.IncrementWhen();
            }

            for (uint8_t idx = 0; idx < _alarmsCount; idx++)
            {
                if (_alarms[idx].period == AlarmPeriod_Expired)
                {
                    _alarms[idx] = alarm;
                    return idx;
                }
            }
            return -1;
        }
        return -2;
    }

    void RemoveAlarm(uint8_t idx)
    {
        if (idx < _alarmsCount)
        {
            _alarms[idx] = AlarmPeriod_Expired;
        }
    }

    void ProcessAlarms()
    {
        uint32_t msNow = millis();
        uint32_t msDelta = (msNow - _msLast);

        if (msDelta > 1000)
        {
            // update seconds based on passed time using millis()
            _seconds += msDelta / 1000;
            _msLast = msNow - (msDelta % 1000); // retain fractional second

            for (uint8_t idx = 0; idx < _alarmsCount; idx++)
            {
                if (_alarms[idx].period != AlarmPeriod_Expired)
                {
                    if (_alarms[idx].when <= _seconds)
                    {
                        if (_alarms[idx].IsSingleFire)
                        {
                            // remove from list
                            _alarms[idx].period = AlarmPeriod_Expired;
                        }
                        else
                        {
                            _alarms[idx].IncrementWhen();
                        }

                        // make callback
                        V_CALLBACK(idx);
                    }
                }
            }
        }
    }

protected:
    struct Alarm
    {
        uint32_t when; // seconds from RtcDateTime.TotalSeconds()
        AlarmPeriod period;  
        
        void IncrementWhen()
        {
            switch (period)
            {
            case AlarmPeriod_Expired:
                break;

            case AlarmPeriod_SingleFire:
                break;

            case AlarmPeriod_Yearly:
                {
                    RtcDateTime temp(when);
                    RtcDateTime next(temp.Year() + 1,
                        temp.Month(),
                        temp.Day(),
                        temp.Hour(),
                        temp.Minute(),
                        temp.Second());
                    when = next.TotalSeconds();
                }
                break;

            case AlarmPeriod_Yearly_Feb29th:
                {
                    RtcDateTime temp(when);
                    uint16_t year = temp.Year() + 1;
                    uint8_t day = 28;

                    if (RtcDateTime::IsLeapYear(year))
                    {
                        day = 29;
                    }

                    RtcDateTime next(year,
                        2,
                        day,
                        temp.Hour(),
                        temp.Minute(),
                        temp.Second());
                    when = next.TotalSeconds();
                }
                break;

            case AlarmPeriod_Monthly:
            case AlarmPeriod_Monthly_29th:
            case AlarmPeriod_Monthly_30th:
            case AlarmPeriod_Monthly_31st:
                {
                    RtcDateTime temp(when);

                    uint16_t year = temp.Year();
                    uint8_t month = temp.Month() + 1;
                    uint8_t day;

                    if (month > 12)
                    {
                        year++;
                        month = 1;
                    }
                                        
                    if (period == AlarmPeriod_Monthly)
                    {
                        // use the day of the month from previous when
                        day = temp.Day();
                    }
                    else
                    {
                        // use the day of the month cached as it may have
                        // been capped to the last day of the month in when
                        day = 29 + (period - AlarmPeriod_Monthly_29th);
                    }

                    uint8_t daysInMonth = RtcDateTime::DaysInMonth(year, month);
                    if (day > daysInMonth)
                    {
                        day = daysInMonth;
                    }

                    RtcDateTime next(year,
                        month,
                        day,
                        temp.Hour(),
                        temp.Minute(),
                        temp.Second());
                    when = next.TotalSeconds();
                }
                break;

            case AlarmPeriod_Weekly:
                when += c_WeekAsSeconds;
                break;

            case AlarmPeriod_Daily:
                when += c_DayAsSeconds;
                break;

            case AlarmPeriod_Hourly:
                when += c_HourAsSeconds;
                break;
            }
        }
    };

    Alarm* _alarms;
    uint8_t _alarmsCount;
    uint32_t _msLast;
    uint32_t _seconds;
};

