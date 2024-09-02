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

#if defined(RTC_NO_STL)

typedef void(*RtcAlarmCallback)(void* context, uint8_t id, const RtcDateTime& alarm);

#else

#undef max
#undef min
#include <functional>
typedef std::function<void(void* context, uint8_t id, const RtcDateTime& alarm)> RtcAlarmCallback;

#endif

enum AlarmPeriod
{
    AlarmPeriod_Expired,
    AlarmPeriod_SingleFire,
    AlarmPeriod_Yearly, 
    AlarmPeriod_Monthly, 
    AlarmPeriod_Monthly_LastDay,
    AlarmPeriod_Weekly, 
    AlarmPeriod_Daily,
    AlarmPeriod_Hourly,
    // the below values are for internal use only
    AlarmPeriod_Yearly_Feb29th, // last of month in Feb if days less than and not a leap year, 
    AlarmPeriod_Monthly_29th, // last of month if days less than, 
    AlarmPeriod_Monthly_30th, // otherwise the day of month matching,
    AlarmPeriod_Monthly_31st, // this will be set internally, just use monthly
    AlarmPeriod_StartOfSpecifics = 60 // anything over this is considered a specific time in seconds
};

enum AlarmAddError
{
    AlarmAddError_PeriodInvalid = -4,
    AlarmAddError_TimePast,
    AlarmAddError_TimeInvalid,
    AlarmAddError_CountExceeded,
};

class RtcAlarmManager
{
public:
    // This class is not meant to be copied nor duplicated
    // 
    // no copy/move constructor
    RtcAlarmManager(const RtcAlarmManager& other) = delete;
    RtcAlarmManager(RtcAlarmManager&& other) noexcept = delete;
    // no copy/move assignment
    RtcAlarmManager& operator=(const RtcAlarmManager& other) = delete;
    RtcAlarmManager& operator=(RtcAlarmManager&& other) noexcept = delete;
    void operator=(RtcAlarmManager& other) = delete;
    // no empty constructor with assignment
    // RtcAlarmManager v = RtcAlarmManager(); // may consider it a functor call
    //
    void operator()() = delete;

    // construct
    RtcAlarmManager() :
        _alarms(nullptr),
        _alarmsCount(0),
        _msLast(0),
        _seconds(0)
    {
    }

    ~RtcAlarmManager()
    {
        Serial.print("~RtcAlarmManager (");
        Serial.print((uint32_t)_alarms, HEX);
        Serial.println(")");

        delete[] _alarms;
    }

    void Begin(uint8_t count)
    {
        if (count > _alarmsCount)
        {
            _alarmsCount = count;
            delete [] _alarms;
            _alarms = new Alarm[_alarmsCount];

            Serial.print("RtcAlarmManager (");
            Serial.print((uint32_t)_alarms, HEX);
            Serial.println(")");

            _msLast = millis();
            _seconds = 0;
        }
    }

    // Expand the number of alarms the manager can handle
    // You can never compress, so use this rarely
    //
    void Expand(uint8_t count)
    {
        if (count > _alarmsCount)
        {
            Alarm* alarmsOld = _alarms;
            _alarms = nullptr;
            Alarm* alarms = new Alarm[count];

            // copy existing
            for (uint8_t alarm = 0; alarm < _alarmsCount; alarm++)
            {
                alarms[alarm] = alarmsOld[alarm];
            }

            delete [] alarmsOld;
            _alarms = alarms;
            _alarmsCount = count;
        }
    }

    // Sync the time to the external trusted source, like
    // a RTC module
    // Do this at regular intervals as the internal CPU timing
    // is not very accurate
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

    // retrieve what the current time the AlarmManager thinks it is
    // due to inaccuracy of the CPU timing this may not be exact,
    // but it is good enough for most timing needs
    // regular use of Sync() will improve this
    RtcDateTime Now() const
    {
        uint32_t msNow = millis();
        uint32_t secondsNow = _seconds + (msNow - _msLast) / 1000;
        return RtcDateTime(secondsNow);
    }

    // add an alarm
    // when - the date and time to start triggering alarms
    // period - the type of alarm, does it repeat and how often, see AlarmPeriod enum
    // return - if positive, the id of the Alarm, otherwise see AlarmAddError
    int8_t AddAlarm(const RtcDateTime& when,
        uint32_t period)
    {
        if (!when.IsValid())
        {
            return AlarmAddError_TimeInvalid;
        }
        if (period > AlarmPeriod_Monthly_31st &&
            period < AlarmPeriod_StartOfSpecifics)
        {
            return AlarmAddError_PeriodInvalid;
        }

        int8_t result = AlarmAddError_TimeInvalid;
        uint32_t seconds = when.TotalSeconds();

        if (period == AlarmPeriod_Monthly_LastDay)
        {
            period = AlarmPeriod_Monthly_31st;
            // adjust given when to last day of its set month
            uint8_t daysInMonth = RtcDateTime::DaysInMonth(when.Year(), when.Month());
            if (when.Day() < daysInMonth)
            {
                RtcDateTime temp(when.Year(),
                    when.Month(),
                    daysInMonth,
                    when.Hour(),
                    when.Minute(),
                    when.Second());
                seconds = temp.TotalSeconds();
            }
        }
        else if (period == AlarmPeriod_Monthly ||
            (period >= AlarmPeriod_Monthly_29th && period <= AlarmPeriod_Monthly_31st))
        {
            period = AlarmPeriod_Monthly;
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
                // for when Feb 29th is target but following year isn't 
                // a leap year it will trigger on the last day of Feb but
                // retain and trigger on specific day of month when
                // available
                period = AlarmPeriod_Yearly_Feb29th;
            }
        }

        Alarm alarm(seconds, period);

        // if the alarm was added that was already in the past,
        // we increment the when to the next repeat
        // for non-repeatable alarms this may expire them
        if (seconds <= _seconds)
        {
            alarm.IncrementWhen();
        }

        if (alarm.Period == AlarmPeriod_Expired)
        {
            result = AlarmAddError_TimePast;
        }
        else
        {
            result = AlarmAddError_CountExceeded;

            for (uint8_t id = 0; id < _alarmsCount; id++)
            {
                if (_alarms[id].Period == AlarmPeriod_Expired)
                {
                    _alarms[id] = alarm;
                    result = id;
                    break;
                }
            }
        }
        
        return result;
    }

    // remove an existing alarm 
    // id - previously returned id from AddAlarm()
    void RemoveAlarm(uint8_t id)
    {
        if (id < _alarmsCount)
        {
            _alarms[id].Period = AlarmPeriod_Expired;
        }
    }

    // check if the alarm is still active 
    // id - previously returned id from AddAlarm()
    bool IsAlarmActive(uint8_t id)
    {
        if (id < _alarmsCount)
        {
            return (_alarms[id].Period != AlarmPeriod_Expired);
        }
        return false;
    }

    // process all the alarms which can trigger callbacks
    // call at regular intervals, if you need seconds accuracy, call
    // every second.  
    // There is little need to call this faster than a few
    // times per second but it doesn't hurt anything
    void ProcessAlarms(RtcAlarmCallback callback, void* context)
    {
        uint32_t msNow = millis();
        uint32_t msDelta = (msNow - _msLast);

        if (msDelta > 1000)
        {
            // update seconds based on passed time using millis()
            _seconds += msDelta / 1000;
            _msLast = msNow - (msDelta % 1000); // retain fractional second
            
            // used a local seconds in case a callback changes it
            uint32_t seconds = _seconds; 

            for (uint8_t id = 0; id < _alarmsCount; id++)
            {
                if (_alarms[id].Period != AlarmPeriod_Expired)
                {
                    if (_alarms[id].When <= seconds)
                    {
                        RtcDateTime alarm(_alarms[id].When);

                        if (_alarms[id].Period == AlarmPeriod_SingleFire)
                        {
                            // remove from list
                            _alarms[id].Period = AlarmPeriod_Expired;
                        }
                        else
                        {
                            _alarms[id].IncrementWhen();
                        }

                        // make callback
                        callback(context, id, alarm);
                    }
                }
            }
        }
    }

protected:
    struct Alarm
    {
        uint32_t When; // seconds from RtcDateTime.TotalSeconds()
        uint32_t Period;  
        
        Alarm(uint32_t when = 0, uint32_t period = AlarmPeriod_Expired) :
            When(when),
            Period(period)
        {
        }

        void IncrementWhen()
        {
            switch (Period)
            {
            case AlarmPeriod_Expired:
                break;

            case AlarmPeriod_SingleFire:
                Period = AlarmPeriod_Expired;
                break;

            case AlarmPeriod_Yearly:
                {
                    RtcDateTime temp(When);
                    RtcDateTime next(temp.Year() + 1,
                        temp.Month(),
                        temp.Day(),
                        temp.Hour(),
                        temp.Minute(),
                        temp.Second());
                    When = next.TotalSeconds();
                }
                break;

            case AlarmPeriod_Yearly_Feb29th:
                {
                    RtcDateTime temp(When);
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
                    When = next.TotalSeconds();
                }
                break;

            case AlarmPeriod_Monthly:
            case AlarmPeriod_Monthly_29th:
            case AlarmPeriod_Monthly_30th:
            case AlarmPeriod_Monthly_31st:
                {
                    RtcDateTime temp(When);

                    uint16_t year = temp.Year();
                    uint8_t month = temp.Month() + 1;
                    uint8_t day;

                    if (month > 12)
                    {
                        year++;
                        month = 1;
                    }
                                        
                    if (Period == AlarmPeriod_Monthly)
                    {
                        // use the day of the month from previous When
                        day = temp.Day();
                    }
                    else
                    {
                        // use the day of the month cached as it may have
                        // been capped to the last day of the month in When
                        day = 29 + (Period - AlarmPeriod_Monthly_29th);
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
                    When = next.TotalSeconds();
                }
                break;

            case AlarmPeriod_Weekly:
                When += c_WeekAsSeconds;
                break;

            case AlarmPeriod_Daily:
                When += c_DayAsSeconds;
                break;

            case AlarmPeriod_Hourly:
                When += c_HourAsSeconds;
                break;

            default:
                When += Period;
                break;
            }
        }
    };

    Alarm* _alarms; // table of possible alarms
    uint8_t _alarmsCount; // max alarms in _alarms
    uint32_t _msLast; // the last call to millis()
    uint32_t _seconds; // the approximate date time, as seconds from 2000
};

