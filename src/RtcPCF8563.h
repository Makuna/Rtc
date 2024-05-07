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


//I2C Slave Address
const uint8_t PCF8563_ADDRESS = 0x51;

//PCF8563 Register Addresses
const uint8_t PCF8563_REG_CONTROL = 0x00;
const uint8_t PCF8563_REG_STATUS = 0x01;
const uint8_t PCF8563_REG_TIMEDATE  = 0x02;
const uint8_t PCF8563_REG_INTEGRITY = 0x02;
const uint8_t PCF8563_REG_ALARM  = 0x09;
const uint8_t PCF8563_REG_CLKOUT_CONTROL = 0x0D;
const uint8_t PCF8563_REG_TIMER_CONTROL = 0x0E;
const uint8_t PCF8563_REG_TIMER  = 0x0F;

//PCF8563 Register Data Size if not just 1
const size_t PCF8563_REG_TIMEDATE_SIZE = 7;
const size_t PCF8563_REG_ALARM_SIZE = 4;

// PCF8563 Valid Register Bits (PCF8563_REG_INTEGRITY)
const uint8_t PCF8563_INTEGRITY_VALID = 7;

// PCF8563 Control Register Bits (PCF8563_REG_CONTROL)
const uint8_t PCF8563_CONTROL_CLOCK = 5;

// PCF8563 Status Register Bits (PCF8563_REG_STATUS)
const uint8_t PCF8563_STATUS_TIE = 0; // timer interrupt enable
const uint8_t PCF8563_STATUS_AIE = 1; // alarm interrupt enable
const uint8_t PCF8563_STATUS_TF = 2; // timer flag
const uint8_t PCF8563_STATUS_AF = 3; // alarm flag
const uint8_t PCF8563_STATUS_TITP = 4; // timer interrupt period config


enum PCF8563SquareWavePinMode
{
    PCF8563SquareWavePinMode_None  = 0b00000000,
    PCF8563SquareWavePinMode_32kHz = 0b10000000,
    PCF8563SquareWavePinMode_1kHz  = 0b10000001,
    PCF8563SquareWavePinMode_32Hz  = 0b10000010,
    PCF8563SquareWavePinMode_1Hz   = 0b10000011
};

// defines the timer period for the timer and
// when to trigger, minutes means at the top of the minute
// and period of 60 seconds
//
enum PCF8563TimerMode
{
    PCF8563TimerMode_None            = 0b00000000,
    PCF8563TimerMode_4096thOfASecond = 0b10000000,
    PCF8563TimerMode_64thOfASecond   = 0b10000001,
    PCF8563TimerMode_Seconds         = 0b10000010,
    PCF8563TimerMode_Minutes         = 0b10000011 // at the top of and by
};

enum PCF8563AlarmControlFlags
{
    PCF8563AlarmControl_MinuteMatch = 0x01,
    PCF8563AlarmControl_HourMatch = 0x02,
    PCF8563AlarmControl_DayOfMonthMatch = 0x04,
    PCF8563AlarmControl_DayOfWeekMatch = 0x08
};

class PCF8563Alarm
{
public:
    PCF8563Alarm(uint8_t dayOfMonth,
        uint8_t hour,
        uint8_t minute,
        uint8_t dayOfWeek,
        uint8_t controlFlags) :
        _flags(controlFlags),
        _dayOfMonth(dayOfMonth),
        _hour(hour),
        _minute(minute),
        _dayOfWeek(dayOfWeek)
    {
    }

    uint8_t DayOfMonth() const
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

    uint8_t DayOfWeek() const
    {
        return _dayOfWeek;
    }

    uint8_t ControlFlags() const
    {
        return _flags;
    }

    bool operator == (const PCF8563Alarm& other) const
    {
        return (_dayOfMonth == other._dayOfMonth &&
            _hour == other._hour &&
            _minute == other._minute &&
            _dayOfWeek == other._dayOfWeek &&
            _flags == other._flags);
    }

    bool operator != (const PCF8563Alarm& other) const
    {
        return !(*this == other);
    }

protected:
    uint8_t _flags;

    uint8_t _dayOfMonth;
    uint8_t _hour;
    uint8_t _minute;
    uint8_t _dayOfWeek;
};

template<class T_WIRE_METHOD> class RtcPCF8563
{
public:
    RtcPCF8563(T_WIRE_METHOD& wire) :
        _wire(wire),
        _lastError(Rtc_Wire_Error_None)
    {
    }

    void Begin()
    {
        _wire.begin();
    }

    void Begin(int sda, int scl)
    {
        _wire.begin(sda, scl);
    }

    uint8_t LastError()
    {
        return _lastError;
    }

    bool IsDateTimeValid()
    {
        uint8_t status = getReg(PCF8563_REG_INTEGRITY);
        return (!(status & _BV(PCF8563_INTEGRITY_VALID)) && (_lastError == Rtc_Wire_Error_None));
    }

    bool GetIsRunning()
    {
        uint8_t creg = getReg(PCF8563_REG_CONTROL);
        return (!(creg & _BV(PCF8563_CONTROL_CLOCK)) && (_lastError == Rtc_Wire_Error_None));
    }

    void SetIsRunning(bool isRunning)
    {
        uint8_t creg = getReg(PCF8563_REG_CONTROL);
        if (isRunning)
        {
            creg &= ~_BV(PCF8563_CONTROL_CLOCK);
        }
        else
        {
            creg |= _BV(PCF8563_CONTROL_CLOCK);
        }
        setReg(PCF8563_REG_CONTROL, creg);
    }

    void SetDateTime(const RtcDateTime& dt)
    {
        // the invalid flag is part of seconds,
        // so is automatically cleared when it is set

        // set the date time
        _wire.beginTransmission(PCF8563_ADDRESS);
        _wire.write(PCF8563_REG_TIMEDATE);

        _wire.write(Uint8ToBcd(dt.Second()));
        _wire.write(Uint8ToBcd(dt.Minute()));
        _wire.write(Uint8ToBcd(dt.Hour())); // 24 hour mode only
        _wire.write(Uint8ToBcd(dt.Day()));
        // RTC Hardware Day of Week is the same as RtcDateTime
        _wire.write(dt.DayOfWeek());

        uint8_t year = dt.Year() - 2000;
        uint8_t centuryFlag = 0;

        if (year >= 100)
        {
            year -= 100;
            centuryFlag = _BV(7);
        }

        _wire.write(Uint8ToBcd(dt.Month()) | centuryFlag);
        _wire.write(Uint8ToBcd(year));

        _lastError = _wire.endTransmission();
    }

    RtcDateTime GetDateTime()
    {
        _wire.beginTransmission(PCF8563_ADDRESS);
        _wire.write(PCF8563_REG_TIMEDATE);
        _lastError = _wire.endTransmission();
        if (_lastError != Rtc_Wire_Error_None)
        {
            return RtcDateTime(0);
        }

        size_t bytesRead = _wire.requestFrom(PCF8563_ADDRESS, PCF8563_REG_TIMEDATE_SIZE);
        if (PCF8563_REG_TIMEDATE_SIZE != bytesRead)
        {
            _lastError = Rtc_Wire_Error_Unspecific;
            return RtcDateTime(0);
        }

        uint8_t second = BcdToUint8(_wire.read() & 0x7F);
        uint8_t minute = BcdToUint8(_wire.read() & 0x7F);
        uint8_t hour = BcdToBin24Hour(_wire.read() & 0x3F);
        uint8_t dayOfMonth = BcdToUint8(_wire.read() & 0x3F);
        _wire.read();  // throwing away day of week as we calculate it
        uint8_t monthRaw = _wire.read();
        uint16_t year = BcdToUint8(_wire.read()) + 2000;

        if (monthRaw & _BV(7)) // century wrap flag
        {
            year += 100;
        }
        uint8_t month = BcdToUint8(monthRaw & 0x1F);


        return RtcDateTime(year, month, dayOfMonth, hour, minute, second);
    }

    void SetSquareWavePin(PCF8563SquareWavePinMode pinMode)
    {
        setReg(PCF8563_REG_CLKOUT_CONTROL, pinMode);
    }

    void SetAlarm(const PCF8563Alarm& alarm)
    {
        _wire.beginTransmission(PCF8563_ADDRESS);
        _wire.write(PCF8563_REG_ALARM);

        uint8_t matchFlag = (alarm.ControlFlags() & PCF8563AlarmControl_MinuteMatch) ? 0x00 : 0x80;
        _wire.write(Uint8ToBcd(alarm.Minute()) | matchFlag);

        matchFlag = (alarm.ControlFlags() & PCF8563AlarmControl_HourMatch) ? 0x00 : 0x80;
        _wire.write(Uint8ToBcd(alarm.Hour()) | matchFlag); // 24 hour mode only

        matchFlag = (alarm.ControlFlags() & PCF8563AlarmControl_DayOfMonthMatch) ? 0x00 : 0x80;
        _wire.write(Uint8ToBcd(alarm.DayOfMonth()) | matchFlag);

        matchFlag = (alarm.ControlFlags() & PCF8563AlarmControl_DayOfWeekMatch) ? 0x00 : 0x80;
        _wire.write(Uint8ToBcd(alarm.DayOfWeek()) | matchFlag);

        _lastError = _wire.endTransmission();

        if (_lastError == Rtc_Wire_Error_None)
        {
            // enable alarm
            uint8_t sreg = getReg(PCF8563_REG_STATUS);
            sreg &= ~_BV(PCF8563_STATUS_AF);
            sreg |= _BV(PCF8563_STATUS_AIE);
            setReg(PCF8563_REG_STATUS, sreg);
        }
    }

    void StopAlarm()
    {
        uint8_t sreg = getReg(PCF8563_REG_STATUS);
        sreg &= ~(_BV(PCF8563_STATUS_AIE) | _BV(PCF8563_STATUS_AF));
        setReg(PCF8563_REG_STATUS, sreg);
    }

    void SetTimer(PCF8563TimerMode mode, uint8_t time)
    {
        _wire.beginTransmission(PCF8563_ADDRESS);
        _wire.write(PCF8563_REG_TIMER_CONTROL);

        _wire.write(mode);
        _wire.write(time);

        _lastError = _wire.endTransmission();

        if (_lastError == Rtc_Wire_Error_None)
        {
            // enable timer
            uint8_t sreg = getReg(PCF8563_REG_STATUS);
            sreg &= ~_BV(PCF8563_STATUS_TF);
            sreg |= _BV(PCF8563_STATUS_TIE) | _BV(PCF8563_STATUS_TITP);
            setReg(PCF8563_REG_STATUS, sreg);
        }
    }

    void StopTimer()
    {
        uint8_t sreg = getReg(PCF8563_REG_STATUS);
        sreg &= ~_BV(PCF8563_STATUS_TF);
        sreg &= ~(_BV(PCF8563_STATUS_TIE) | _BV(PCF8563_STATUS_TITP));
        setReg(PCF8563_REG_STATUS, sreg);

        if (_lastError == Rtc_Wire_Error_None)
        {
            setReg(PCF8563_REG_TIMER_CONTROL, PCF8563TimerMode_None);
        }
    }

    // Latch must be called after an alarm otherwise it will not
    // trigger again?
    bool LatchAlarmTriggeredFlag()
    {
        uint8_t sreg = getReg(PCF8563_REG_STATUS);
        bool triggered = false;
        if (sreg & _BV(PCF8563_STATUS_AF))
        {
            triggered = true;
            sreg &= ~_BV(PCF8563_STATUS_AF);
            setReg(PCF8563_REG_STATUS, sreg);
        }
        return triggered;
    }

    bool LatchTimerTriggeredFlag()
    {
        uint8_t sreg = getReg(PCF8563_REG_STATUS);
        bool triggered = false;
        if (sreg & _BV(PCF8563_STATUS_TF))
        {
            triggered = true;
            sreg &= ~_BV(PCF8563_STATUS_TF);
            setReg(PCF8563_REG_STATUS, sreg);
        }
        return triggered;
    }

    bool GetAlarmTriggered()
    {
        uint8_t sreg = getReg(PCF8563_REG_STATUS);
        return (sreg & _BV(PCF8563_STATUS_AF));
    }

    bool GetTimerTriggered()
    {
        uint8_t sreg = getReg(PCF8563_REG_STATUS);
        return (sreg & _BV(PCF8563_STATUS_TF));
    }

private:
    T_WIRE_METHOD& _wire;
    uint8_t _lastError;

    uint8_t getReg(uint8_t regAddress)
    {
        _wire.beginTransmission(PCF8563_ADDRESS);
        _wire.write(regAddress);
        _lastError = _wire.endTransmission();
        if (_lastError != Rtc_Wire_Error_None)
        {
            return 0;
        }

        // control register
        size_t bytesRead = _wire.requestFrom(PCF8563_ADDRESS, (uint8_t)1);
        if (1 != bytesRead)
        {
            _lastError = Rtc_Wire_Error_Unspecific;
            return 0;
        }

        uint8_t regValue = _wire.read();
        return regValue;
    }

    void setReg(uint8_t regAddress, uint8_t regValue)
    {
        _wire.beginTransmission(PCF8563_ADDRESS);
        _wire.write(regAddress);
        _wire.write(regValue);
        _lastError = _wire.endTransmission();
    }

};

