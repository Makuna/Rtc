

#ifndef __RTCDS3231_H__
#define __RTCDS3231_H__

#include <Arduino.h>

#include <RtcDateTime.h>
#include <RtcTemperature.h>

// seconds accuracy
enum DS3231AlarmOneControl
{
    // bit order:  A1M4  DY/DT  A1M3  A1M2  A1M1
    DS3231AlarmOneControl_HoursMinutesSecondsDayOfMonthMatch = 0x00,
    DS3231AlarmOneControl_OncePerSecond = 0x17,
    DS3231AlarmOneControl_SecondsMatch = 0x16,
    DS3231AlarmOneControl_MinutesSecondsMatch = 0x14,
    DS3231AlarmOneControl_HoursMinutesSecondsMatch = 0x10,
    DS3231AlarmOneControl_HoursMinutesSecondsDayOfWeekMatch = 0x08,
};

class DS3231AlarmOne 
{
public:
    DS3231AlarmOne( uint8_t dayOf,
            uint8_t hour,
            uint8_t minute,
            uint8_t second,
            DS3231AlarmOneControl controlFlags) :
        _flags(controlFlags),
        _dayOf(dayOf),
        _hour(hour),
        _minute(minute),
        _second(second)
    {
    }

    uint8_t DayOf() const
    {
        return _dayOf;
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

    DS3231AlarmOneControl ControlFlags() const
    {
        return _flags;
    }

    bool operator == (const DS3231AlarmOne& other) const
    {
        return (_dayOf == other._dayOf &&
                _hour == other._hour &&
                _minute == other._minute &&
                _second == other._second &&
                _flags == other._flags);
    }

    bool operator != (const DS3231AlarmOne& other) const
    {
        return !(*this == other);
    }

protected:
    uint8_t _second;
    uint8_t _minute;
    uint8_t _hour;
    uint8_t _dayOf;

    DS3231AlarmOneControl _flags;
};

// minutes accuracy
enum DS3231AlarmTwoControl
{
    // bit order:  A2M4  DY/DT  A2M3  A2M2
    DS3231AlarmTwoControl_HoursMinutesDayOfMonthMatch = 0x00,
    DS3231AlarmTwoControl_OncePerMinute = 0x0b,
    DS3231AlarmTwoControl_MinutesMatch = 0x0a,
    DS3231AlarmTwoControl_HoursMinutesMatch = 0x08,
    DS3231AlarmTwoControl_HoursMinutesDayOfWeekMatch = 0x04,
};

class DS3231AlarmTwo
{
public:
    DS3231AlarmTwo( uint8_t dayOf,
            uint8_t hour,
            uint8_t minute,
            DS3231AlarmTwoControl controlFlags) :
        _flags(controlFlags),
        _dayOf(dayOf),
        _hour(hour),
        _minute(minute)
    {
    }

    uint8_t DayOf() const
    {
        return _dayOf;
    }

    uint8_t Hour() const
    {
        return _hour;
    }

    uint8_t Minute() const
    {
        return _minute;
    }

    DS3231AlarmTwoControl ControlFlags() const
    {
        return _flags;
    }

    bool operator == (const DS3231AlarmTwo& other) const
    {
        return (_dayOf == other._dayOf &&
                _hour == other._hour &&
                _minute == other._minute &&
                _flags == other._flags);
    }

    bool operator != (const DS3231AlarmTwo& other) const
    {
        return !(*this == other);
    }

protected:
    uint8_t _minute;
    uint8_t _hour;
    uint8_t _dayOf;

    DS3231AlarmTwoControl _flags;
};


enum DS3231SquareWaveClock
{
    DS3231SquareWaveClock_1Hz  = 0b00000000,
    DS3231SquareWaveClock_1kHz = 0b00001000,
    DS3231SquareWaveClock_4kHz = 0b00010000,
    DS3231SquareWaveClock_8kHz = 0b00011000,
};

enum DS3231SquareWavePinMode
{
    DS3231SquareWavePin_ModeNone,
    DS3231SquareWavePin_ModeBatteryBackup,
    DS3231SquareWavePin_ModeClock,
    DS3231SquareWavePin_ModeAlarmOne,
    DS3231SquareWavePin_ModeAlarmTwo,
    DS3231SquareWavePin_ModeAlarmBoth
};

enum DS3231AlarmFlag
{
    DS3231AlarmFlag_Alarm1 = 0x01,
    DS3231AlarmFlag_Alarm2 = 0x02,
    DS3231AlarmFlag_AlarmBoth = 0x03,
};

class RtcDS3231
{
public:
    static void Begin();

    static bool IsDateTimeValid();

    static bool GetIsRunning();	
    static void SetIsRunning(bool isRunning);

    static void SetDateTime(const RtcDateTime& dt);	
    static RtcDateTime GetDateTime();	

    static RtcTemperature GetTemperature();			

    static void Enable32kHzPin(bool enable);	
    static void SetSquareWavePin(DS3231SquareWavePinMode pinMode);

    static void SetSquareWavePinClockFrequency(DS3231SquareWaveClock freq);		

    static void SetAlarmOne(const DS3231AlarmOne& alarm);
    static void SetAlarmTwo(const DS3231AlarmTwo& alarm);
    static DS3231AlarmOne GetAlarmOne();	
    static DS3231AlarmTwo GetAlarmTwo();
    // Latch must be called after an alarm otherwise it will not
    // trigger again
    static DS3231AlarmFlag LatchAlarmsTriggeredFlags();

    static void ForceTemperatureCompensationUpdate(bool block);	

    static int8_t GetAgingOffset();
    static void SetAgingOffset(int8_t value);

private:
    static uint8_t getReg(uint8_t regAddress);
    static void setReg(uint8_t regAddress, uint8_t regValue);
};

#endif // __RTCDS3231_H__