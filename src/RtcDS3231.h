

#ifndef __RTCDS3231_H__
#define __RTCDS3231_H__

#include <Arduino.h>

#include "RtcDateTime.h"
#include "RtcTemperature.h"
#include "RtcUtility.h"


//I2C Slave Address  
const uint8_t DS3231_ADDRESS = 0x68;

//DS3231 Register Addresses
const uint8_t DS3231_REG_TIMEDATE  = 0x00;
const uint8_t DS3231_REG_ALARMONE  = 0x07;
const uint8_t DS3231_REG_ALARMTWO  = 0x0B;
                                         
const uint8_t DS3231_REG_CONTROL   = 0x0E;
const uint8_t DS3231_REG_STATUS    = 0x0F;
const uint8_t DS3231_REG_AGING     = 0x10;
                                         
const uint8_t DS3231_REG_TEMP      = 0x11;

//DS3231 Register Data Size if not just 1
const uint8_t DS3231_REG_TIMEDATE_SIZE = 7;
const uint8_t DS3231_REG_ALARMONE_SIZE = 4;
const uint8_t DS3231_REG_ALARMTWO_SIZE = 3;

const uint8_t DS3231_REG_TEMP_SIZE = 2;

// DS3231 Control Register Bits
const uint8_t DS3231_A1IE  = 0;
const uint8_t DS3231_A2IE  = 1;
const uint8_t DS3231_INTCN = 2;
const uint8_t DS3231_RS1   = 3;
const uint8_t DS3231_RS2   = 4;
const uint8_t DS3231_CONV  = 5;
const uint8_t DS3231_BBSQW = 6;
const uint8_t DS3231_EOSC  = 7;
const uint8_t DS3231_AIEMASK = (_BV(DS3231_A1IE) | _BV(DS3231_A2IE));
const uint8_t DS3231_RSMASK = (_BV(DS3231_RS1) | _BV(DS3231_RS2));

// DS3231 Status Register Bits
const uint8_t DS3231_A1F      = 0;
const uint8_t DS3231_A2F      = 1;
const uint8_t DS3231_BSY      = 2;
const uint8_t DS3231_EN32KHZ  = 3;
const uint8_t DS3231_OSF      = 7;
const uint8_t DS3231_AIFMASK = (_BV(DS3231_A1F) | _BV(DS3231_A2F));


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
    DS3231AlarmOneControl _flags;

	uint8_t _dayOf;
	uint8_t _hour;
	uint8_t _minute;
    uint8_t _second;  
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
    DS3231AlarmTwoControl _flags;

	uint8_t _dayOf;
	uint8_t _hour;
	uint8_t _minute;
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

template<class T_WIRE_METHOD> class RtcDS3231
{
public:
    RtcDS3231(T_WIRE_METHOD& wire) :
        _wire(wire)
    {
    }

    void Begin()
    {
        _wire.begin();
    }

    bool IsDateTimeValid()
    {
        uint8_t status = getReg(DS3231_REG_STATUS);
        return !(status & _BV(DS3231_OSF));
    }

    bool GetIsRunning()
    {
        uint8_t creg = getReg(DS3231_REG_CONTROL);
        return !(creg & _BV(DS3231_EOSC));
    }
    void SetIsRunning(bool isRunning)
    {
        uint8_t creg = getReg(DS3231_REG_CONTROL);
        if (isRunning)
        {
            creg &= ~_BV(DS3231_EOSC);
        }
        else
        {
            creg |= _BV(DS3231_EOSC);
        }
        setReg(DS3231_REG_CONTROL, creg);
    }

    void SetDateTime(const RtcDateTime& dt)
    {
        // clear the invalid flag
        uint8_t status = getReg(DS3231_REG_STATUS);
        status &= ~_BV(DS3231_OSF); // clear the flag
        setReg(DS3231_REG_STATUS, status);

        // set the date time
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_TIMEDATE);

        _wire.write(Uint8ToBcd(dt.Second()));
        _wire.write(Uint8ToBcd(dt.Minute()));
        _wire.write(Uint8ToBcd(dt.Hour())); // 24 hour mode only

        uint8_t year = dt.Year() - 2000;
        uint8_t centuryFlag = 0;

        if (year >= 100)
        {
            year -= 100;
            centuryFlag = _BV(7);
        }

        _wire.write(Uint8ToBcd(dt.DayOfWeek()));
        _wire.write(Uint8ToBcd(dt.Day()));
        _wire.write(Uint8ToBcd(dt.Month()) | centuryFlag);
        _wire.write(Uint8ToBcd(year));

        _wire.endTransmission();
    }
    RtcDateTime GetDateTime()
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_TIMEDATE);
        _wire.endTransmission();

        _wire.requestFrom(DS3231_ADDRESS, DS3231_REG_TIMEDATE_SIZE);
        uint8_t second = BcdToUint8(_wire.read() & 0x7F);
        uint8_t minute = BcdToUint8(_wire.read());
        uint8_t hour = BcdToBin24Hour(_wire.read());

        _wire.read();  // throwing away day of week as we calculate it

        uint8_t dayOfMonth = BcdToUint8(_wire.read());
        uint8_t monthRaw = _wire.read();
        uint16_t year = BcdToUint8(_wire.read()) + 2000;

        if (monthRaw & _BV(7)) // century wrap flag
        {
            year += 100;
        }
        uint8_t month = BcdToUint8(monthRaw & 0x7f);


        return RtcDateTime(year, month, dayOfMonth, hour, minute, second);
    }

    RtcTemperature GetTemperature()
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_TEMP);
        _wire.endTransmission();

        // Temperature is represented as a 10-bit code with a resolution
        // of 1/4th °C and is accessable as a signed 16-bit integer at
        // locations 11h and 12h.
        //
        //       |         r11h          | DP |         r12h         |
        // Bit:   15 14 13 12 11 10  9  8   .  7  6  5  4  3  2  1  0  -1 -2
        //         s  i  i  i  i  i  i  i   .  f  f  0  0  0  0  0  0
        //
        // As it takes (8) right-shifts to register the decimal point (DP) to
        // the right of the 0th bit, the overall word scaling equals 256.
        //
        // For example, at +/- 25.25°C, concatenated registers <r11h:r12h> =
        // 256 * (+/- 25+(1/4)) = +/- 6464, or 1940h / E6C0h.

        _wire.requestFrom(DS3231_ADDRESS, DS3231_REG_TEMP_SIZE);
        int8_t  r11h = _wire.read();                  // MS byte, signed temperature
        return RtcTemperature( r11h, _wire.read() );  // LS byte is r12h
    }

    void Enable32kHzPin(bool enable)
    {
        uint8_t sreg = getReg(DS3231_REG_STATUS);

        if (enable == true)
        {
            sreg |= _BV(DS3231_EN32KHZ);
        }
        else
        {
            sreg &= ~_BV(DS3231_EN32KHZ);
        }

        setReg(DS3231_REG_STATUS, sreg);
    }

    void SetSquareWavePin(DS3231SquareWavePinMode pinMode)
    {
        uint8_t creg = getReg(DS3231_REG_CONTROL);

        // clear all relevant bits to a known "off" state
        creg &= ~(DS3231_AIEMASK | _BV(DS3231_BBSQW));
        creg |= _BV(DS3231_INTCN);  // set INTCN to disables SQW

        switch (pinMode)
        {
        case DS3231SquareWavePin_ModeNone:
            break;

        case DS3231SquareWavePin_ModeBatteryBackup:
            creg |= _BV(DS3231_BBSQW); // set battery backup flag
            creg &= ~_BV(DS3231_INTCN); // clear INTCN to enable SQW 
            break;

        case DS3231SquareWavePin_ModeClock:
            creg &= ~_BV(DS3231_INTCN); // clear INTCN to enable SQW 
            break;

        case DS3231SquareWavePin_ModeAlarmOne:
            creg |= _BV(DS3231_A1IE);
            break;

        case DS3231SquareWavePin_ModeAlarmTwo:
            creg |= _BV(DS3231_A2IE);
            break;

        case DS3231SquareWavePin_ModeAlarmBoth:
            creg |= _BV(DS3231_A1IE) | _BV(DS3231_A2IE);
            break;
        }

        setReg(DS3231_REG_CONTROL, creg);
    }

    void SetSquareWavePinClockFrequency(DS3231SquareWaveClock freq)
    {
        uint8_t creg = getReg(DS3231_REG_CONTROL);

        creg &= ~DS3231_RSMASK; // Set to 0
        creg |= (freq & DS3231_RSMASK); // Set freq bits

        setReg(DS3231_REG_CONTROL, creg);
    }


    void SetAlarmOne(const DS3231AlarmOne& alarm)
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_ALARMONE);

        _wire.write(Uint8ToBcd(alarm.Second()) | ((alarm.ControlFlags() & 0x01) << 7));
        _wire.write(Uint8ToBcd(alarm.Minute()) | ((alarm.ControlFlags() & 0x02) << 6));
        _wire.write(Uint8ToBcd(alarm.Hour()) | ((alarm.ControlFlags() & 0x04) << 5)); // 24 hour mode only

        _wire.write(Uint8ToBcd(alarm.DayOf()) | ((alarm.ControlFlags() & 0x18) << 3));

        _wire.endTransmission();
    }

    void SetAlarmTwo(const DS3231AlarmTwo& alarm)
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_ALARMTWO);

        _wire.write(Uint8ToBcd(alarm.Minute()) | ((alarm.ControlFlags() & 0x01) << 7));
        _wire.write(Uint8ToBcd(alarm.Hour()) | ((alarm.ControlFlags() & 0x02) << 6)); // 24 hour mode only

        _wire.write(Uint8ToBcd(alarm.DayOf()) | ((alarm.ControlFlags() & 0x0c) << 4));

        _wire.endTransmission();
    }

    DS3231AlarmOne GetAlarmOne()
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_ALARMONE);
        _wire.endTransmission();

        _wire.requestFrom(DS3231_ADDRESS, DS3231_REG_ALARMONE_SIZE);

        uint8_t raw = _wire.read();
        uint8_t flags = (raw & 0x80) >> 7;
        uint8_t second = BcdToUint8(raw & 0x7F);

        raw = _wire.read();
        flags |= (raw & 0x80) >> 6;
        uint8_t minute = BcdToUint8(raw & 0x7F);

        raw = _wire.read();
        flags |= (raw & 0x80) >> 5;
        uint8_t hour = BcdToBin24Hour(raw & 0x7f);

        raw = _wire.read();
        flags |= (raw & 0xc0) >> 3;
        uint8_t dayOf = BcdToUint8(raw & 0x3f);

        return DS3231AlarmOne(dayOf, hour, minute, second, (DS3231AlarmOneControl)flags);
    }

    DS3231AlarmTwo GetAlarmTwo()
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(DS3231_REG_ALARMTWO);
        _wire.endTransmission();

        _wire.requestFrom(DS3231_ADDRESS, DS3231_REG_ALARMTWO_SIZE);

        uint8_t raw = _wire.read();
        uint8_t flags = (raw & 0x80) >> 7;
        uint8_t minute = BcdToUint8(raw & 0x7F);

        raw = _wire.read();
        flags |= (raw & 0x80) >> 6;
        uint8_t hour = BcdToBin24Hour(raw & 0x7f);

        raw = _wire.read();
        flags |= (raw & 0xc0) >> 4;
        uint8_t dayOf = BcdToUint8(raw & 0x3f);

        return DS3231AlarmTwo(dayOf, hour, minute, (DS3231AlarmTwoControl)flags);
    }

    // Latch must be called after an alarm otherwise it will not
    // trigger again
    DS3231AlarmFlag LatchAlarmsTriggeredFlags()
    {
        uint8_t sreg = getReg(DS3231_REG_STATUS);
        uint8_t alarmFlags = (sreg & DS3231_AIFMASK);
        sreg &= ~DS3231_AIFMASK; // clear the flags
        setReg(DS3231_REG_STATUS, sreg);
        return (DS3231AlarmFlag)alarmFlags;
    }

    void ForceTemperatureCompensationUpdate(bool block)
    {
        uint8_t creg = getReg(DS3231_REG_CONTROL);
        creg |= _BV(DS3231_CONV); // Write CONV bit
        setReg(DS3231_REG_CONTROL, creg);

        while (block && (creg & _BV(DS3231_CONV)) != 0)
        {
            // Block until CONV is 0
            creg = getReg(DS3231_REG_CONTROL);
        }
    }

    int8_t GetAgingOffset()
    {
        return getReg(DS3231_REG_AGING);
    }

    void SetAgingOffset(int8_t value)
    {
        setReg(DS3231_REG_AGING, value);
    }

private:
    T_WIRE_METHOD& _wire;

    uint8_t getReg(uint8_t regAddress)
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(regAddress);
        _wire.endTransmission();

        // control register
        _wire.requestFrom(DS3231_ADDRESS, (uint8_t)1);

        uint8_t regValue = _wire.read();
        return regValue;
    }

    void setReg(uint8_t regAddress, uint8_t regValue)
    {
        _wire.beginTransmission(DS3231_ADDRESS);
        _wire.write(regAddress);
        _wire.write(regValue);
        _wire.endTransmission();
    }
};

#endif // __RTCDS3231_H__
