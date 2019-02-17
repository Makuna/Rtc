

#ifndef __RTCDS3234_H__
#define __RTCDS3234_H__

#include <Arduino.h>
#include <SPI.h>

#include "RtcDateTime.h"
#include "RtcTemperature.h"
#include "RtcUtility.h"


//DS3234 Register Addresses
const uint8_t DS3234_REG_WRITE_FLAG = 0x80;

const uint8_t DS3234_REG_TIMEDATE  = 0x00;

const uint8_t DS3234_REG_ALARMONE  = 0x07;
const uint8_t DS3234_REG_ALARMTWO  = 0x0B;
                                         
const uint8_t DS3234_REG_CONTROL   = 0x0E;
const uint8_t DS3234_REG_STATUS    = 0x0F;
const uint8_t DS3234_REG_AGING     = 0x10;
                                         
const uint8_t DS3234_REG_TEMP      = 0x11;

const uint8_t DS3234_REG_RAM_ADDRESS = 0x18;
const uint8_t DS3234_REG_RAM_DATA = 0x19;

const uint8_t DS3234_RAMSTART = 0x00;
const uint8_t DS3234_RAMEND = 0xFF;
const uint8_t DS3234_RAMSIZE = DS3234_RAMEND - DS3234_RAMSTART;

// DS3234 Control Register Bits
const uint8_t DS3234_A1IE  = 0;
const uint8_t DS3234_A2IE  = 1;
const uint8_t DS3234_INTCN = 2;
const uint8_t DS3234_RS1   = 3;
const uint8_t DS3234_RS2   = 4;
const uint8_t DS3234_CONV  = 5;
const uint8_t DS3234_BBSQW = 6;
const uint8_t DS3234_EOSC  = 7;
const uint8_t DS3234_AIEMASK = (_BV(DS3234_A1IE) | _BV(DS3234_A2IE));
const uint8_t DS3234_RSMASK = (_BV(DS3234_RS1) | _BV(DS3234_RS2));

// DS3234 Status Register Bits
const uint8_t DS3234_A1F      = 0;
const uint8_t DS3234_A2F      = 1;
const uint8_t DS3234_BSY      = 2;
const uint8_t DS3234_EN32KHZ  = 3;
const uint8_t DS3234_CRATE0   = 4;
const uint8_t DS3234_CRATE1   = 5;
const uint8_t DS3234_BB32KHZ  = 6;
const uint8_t DS3234_OSF      = 7;
const uint8_t DS3234_AIFMASK = (_BV(DS3234_A1F) | _BV(DS3234_A2F));
const uint8_t DS3234_CRATEMASK = (_BV(DS3234_CRATE0) | _BV(DS3234_CRATE1));

// seconds accuracy
enum DS3234AlarmOneControl
{
    // bit order:  A1M4  DY/DT  A1M3  A1M2  A1M1
    DS3234AlarmOneControl_HoursMinutesSecondsDayOfMonthMatch = 0x00,
    DS3234AlarmOneControl_OncePerSecond = 0x17,
    DS3234AlarmOneControl_SecondsMatch = 0x16,
    DS3234AlarmOneControl_MinutesSecondsMatch = 0x14,
    DS3234AlarmOneControl_HoursMinutesSecondsMatch = 0x10,
    DS3234AlarmOneControl_HoursMinutesSecondsDayOfWeekMatch = 0x08,
};

class DS3234AlarmOne
{
public:
    DS3234AlarmOne( uint8_t dayOf,
            uint8_t hour,
            uint8_t minute,
            uint8_t second,
            DS3234AlarmOneControl controlFlags) :
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

    DS3234AlarmOneControl ControlFlags() const
    {
        return _flags;
    }

    bool operator == (const DS3234AlarmOne& other) const
    {
        return (_dayOf == other._dayOf &&
                _hour == other._hour &&
                _minute == other._minute &&
                _second == other._second &&
                _flags == other._flags);
    }

    bool operator != (const DS3234AlarmOne& other) const
    {
        return !(*this == other);
    }

protected:
    DS3234AlarmOneControl _flags;

    uint8_t _dayOf;
    uint8_t _hour;
    uint8_t _minute;
    uint8_t _second;  
};

// minutes accuracy
enum DS3234AlarmTwoControl
{
    // bit order:  A2M4  DY/DT  A2M3  A2M2
    DS3234AlarmTwoControl_HoursMinutesDayOfMonthMatch = 0x00,
    DS3234AlarmTwoControl_OncePerMinute = 0x0b,
    DS3234AlarmTwoControl_MinutesMatch = 0x0a,
    DS3234AlarmTwoControl_HoursMinutesMatch = 0x08,
    DS3234AlarmTwoControl_HoursMinutesDayOfWeekMatch = 0x04,
};

class DS3234AlarmTwo
{
public:
    DS3234AlarmTwo( uint8_t dayOf,
            uint8_t hour,
            uint8_t minute,
            DS3234AlarmTwoControl controlFlags) :
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

    DS3234AlarmTwoControl ControlFlags() const
    {
        return _flags;
    }

    bool operator == (const DS3234AlarmTwo& other) const
    {
        return (_dayOf == other._dayOf &&
                _hour == other._hour &&
                _minute == other._minute &&
                _flags == other._flags);
    }

    bool operator != (const DS3234AlarmTwo& other) const
    {
        return !(*this == other);
    }

protected:
    DS3234AlarmTwoControl _flags;

    uint8_t _dayOf;
    uint8_t _hour;
    uint8_t _minute;
};


enum DS3234SquareWaveClock
{
    DS3234SquareWaveClock_1Hz  = 0b00000000,
    DS3234SquareWaveClock_1kHz = 0b00001000,
    DS3234SquareWaveClock_4kHz = 0b00010000,
    DS3234SquareWaveClock_8kHz = 0b00011000,
};

enum DS3234SquareWavePinMode
{
    DS3234SquareWavePin_ModeNone,
    DS3234SquareWavePin_ModeBatteryBackup,
    DS3234SquareWavePin_ModeClock,
    DS3234SquareWavePin_ModeAlarmOne,
    DS3234SquareWavePin_ModeAlarmTwo,
    DS3234SquareWavePin_ModeAlarmBoth
};

enum DS3234TempCompensationRate
{
    DS3234TempCompensationRate_64Seconds,
    DS3234TempCompensationRate_128Seconds,
    DS3234TempCompensationRate_256Seconds,
    DS3234TempCompensationRate_512Seconds,
};

enum DS3234AlarmFlag
{
    DS3234AlarmFlag_Alarm1 = 0x01,
    DS3234AlarmFlag_Alarm2 = 0x02,
    DS3234AlarmFlag_AlarmBoth = 0x03,
};

const SPISettings c_Ds3234SpiSettings(1000000, MSBFIRST, SPI_MODE1); // CPHA must be used, so mode 1 or mode 3 are valid

template<class T_SPI_METHOD> class RtcDS3234
{
public:
    RtcDS3234(T_SPI_METHOD& spi, uint8_t csPin) :
        _spi(spi),
        _csPin(csPin)
    {
    }

    void Begin()
    {
        UnselectChip();
        pinMode(_csPin, OUTPUT);
    }

    bool IsDateTimeValid()
    {
        uint8_t status = getReg(DS3234_REG_STATUS);
        return !(status & _BV(DS3234_OSF));
    }

    bool GetIsRunning()
    {
        uint8_t creg = getReg(DS3234_REG_CONTROL);
        return !(creg & _BV(DS3234_EOSC));
    }

    void SetIsRunning(bool isRunning)
    {
        uint8_t creg = getReg(DS3234_REG_CONTROL);
        if (isRunning)
        {
            creg &= ~_BV(DS3234_EOSC);
        }
        else
        {
            creg |= _BV(DS3234_EOSC);
        }
        setReg(DS3234_REG_CONTROL, creg);
    }

    void SetDateTime(const RtcDateTime& dt)
    {
        // clear the invalid flag
        uint8_t status = getReg(DS3234_REG_STATUS);
        status &= ~_BV(DS3234_OSF); // clear the flag
        setReg(DS3234_REG_STATUS, status);

        // set the date time
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();
        _spi.transfer(DS3234_REG_TIMEDATE | DS3234_REG_WRITE_FLAG);

        _spi.transfer(Uint8ToBcd(dt.Second()));
        _spi.transfer(Uint8ToBcd(dt.Minute()));
        _spi.transfer(Uint8ToBcd(dt.Hour())); // 24 hour mode only

        uint8_t year = dt.Year() - 2000;
        uint8_t centuryFlag = 0;

        if (year >= 100)
        {
            year -= 100;
            centuryFlag = _BV(7);
        }

        // RTC Hardware Day of Week is 1-7, 1 = Monday
        // convert our Day of Week to Rtc Day of Week
        uint8_t rtcDow = RtcDateTime::ConvertDowToRtc(dt.DayOfWeek());

        _spi.transfer(Uint8ToBcd(rtcDow));

        _spi.transfer(Uint8ToBcd(dt.Day()));
        _spi.transfer(Uint8ToBcd(dt.Month()) | centuryFlag);
        _spi.transfer(Uint8ToBcd(year));
        UnselectChip();
        _spi.endTransaction();
    }

    RtcDateTime GetDateTime()
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();

        _spi.transfer(DS3234_REG_TIMEDATE);

        uint8_t second = BcdToUint8(_spi.transfer(0));
        uint8_t minute = BcdToUint8(_spi.transfer(0));
        uint8_t hour = BcdToBin24Hour(_spi.transfer(0));

        _spi.transfer(0); // throwing away day of week as we calculate it

        uint8_t dayOfMonth = BcdToUint8(_spi.transfer(0));
        uint8_t monthRaw = _spi.transfer(0);
        uint16_t year = BcdToUint8(_spi.transfer(0)) + 2000;

        UnselectChip();
        _spi.endTransaction();

        if (monthRaw & _BV(7)) // century wrap flag
        {
            year += 100;
        }
        uint8_t month = BcdToUint8(monthRaw & 0x7f);

        return RtcDateTime(year, month, dayOfMonth, hour, minute, second);
    }

    RtcTemperature GetTemperature()
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();
        _spi.transfer(DS3234_REG_TEMP);

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

        int8_t  ms = _spi.transfer(0);  // MS byte, signed temperature
        uint8_t ls = _spi.transfer(0);  // LS byte is r12h

        UnselectChip();
        _spi.endTransaction();

        return RtcTemperature(ms, ls);  // LS byte is r12h
    }

    void Enable32kHzPin(bool enable)
    {
        uint8_t sreg = getReg(DS3234_REG_STATUS);

        if (enable == true)
        {
            sreg |= _BV(DS3234_EN32KHZ);
        }
        else
        {
            sreg &= ~_BV(DS3234_EN32KHZ);
        }

        setReg(DS3234_REG_STATUS, sreg);
    }

    void SetSquareWavePin(DS3234SquareWavePinMode pinMode)
    {
        uint8_t creg = getReg(DS3234_REG_CONTROL);

        // clear all relevant bits to a known "off" state
        creg &= ~(DS3234_AIEMASK | _BV(DS3234_BBSQW));
        creg |= _BV(DS3234_INTCN);  // set INTCN to disables SQW

        switch (pinMode)
        {
        case DS3234SquareWavePin_ModeNone:
            break;

        case DS3234SquareWavePin_ModeBatteryBackup:
            creg |= _BV(DS3234_BBSQW); // set battery backup flag
            creg &= ~_BV(DS3234_INTCN); // clear INTCN to enable SQW 
            break;

        case DS3234SquareWavePin_ModeClock:
            creg &= ~_BV(DS3234_INTCN); // clear INTCN to enable SQW 
            break;

        case DS3234SquareWavePin_ModeAlarmOne:
            creg |= _BV(DS3234_A1IE);
            break;

        case DS3234SquareWavePin_ModeAlarmTwo:
            creg |= _BV(DS3234_A2IE);
            break;

        case DS3234SquareWavePin_ModeAlarmBoth:
            creg |= _BV(DS3234_A1IE) | _BV(DS3234_A2IE);
            break;
        }

        setReg(DS3234_REG_CONTROL, creg);
    }

    void SetSquareWavePinClockFrequency(DS3234SquareWaveClock freq)
    {
        uint8_t creg = getReg(DS3234_REG_CONTROL);

        creg &= ~DS3234_RSMASK; // Set to 0
        creg |= (freq & DS3234_RSMASK); // Set freq bits

        setReg(DS3234_REG_CONTROL, creg);
    }


    void SetAlarmOne(const DS3234AlarmOne& alarm)
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();

        _spi.transfer(DS3234_REG_ALARMONE | DS3234_REG_WRITE_FLAG);

        _spi.transfer(Uint8ToBcd(alarm.Second()) | ((alarm.ControlFlags() & 0x01) << 7));
        _spi.transfer(Uint8ToBcd(alarm.Minute()) | ((alarm.ControlFlags() & 0x02) << 6));
        _spi.transfer(Uint8ToBcd(alarm.Hour()) | ((alarm.ControlFlags() & 0x04) << 5)); // 24 hour mode only

        uint8_t rtcDow = alarm.DayOf();
        if (alarm.ControlFlags() == DS3234AlarmOneControl_HoursMinutesSecondsDayOfWeekMatch)
        {
            rtcDow = RtcDateTime::ConvertDowToRtc(rtcDow);
        }

        _spi.transfer(Uint8ToBcd(rtcDow) | ((alarm.ControlFlags() & 0x18) << 3));

        UnselectChip();
        _spi.endTransaction();
    }

    void SetAlarmTwo(const DS3234AlarmTwo& alarm)
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();

        _spi.transfer(DS3234_REG_ALARMTWO | DS3234_REG_WRITE_FLAG);

        _spi.transfer(Uint8ToBcd(alarm.Minute()) | ((alarm.ControlFlags() & 0x01) << 7));
        _spi.transfer(Uint8ToBcd(alarm.Hour()) | ((alarm.ControlFlags() & 0x02) << 6)); // 24 hour mode only

        // convert our Day of Week to Rtc Day of Week if needed
        uint8_t rtcDow = alarm.DayOf();
        if (alarm.ControlFlags() == DS3234AlarmTwoControl_HoursMinutesDayOfWeekMatch)
        {
            rtcDow = RtcDateTime::ConvertDowToRtc(rtcDow);
        }
        
        _spi.transfer(Uint8ToBcd(rtcDow) | ((alarm.ControlFlags() & 0x0c) << 4));

        UnselectChip();
        _spi.endTransaction();
    }

    DS3234AlarmOne GetAlarmOne()
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();

        _spi.transfer(DS3234_REG_ALARMONE);
  
        uint8_t raw = _spi.transfer(0);
        uint8_t flags = (raw & 0x80) >> 7;
        uint8_t second = BcdToUint8(raw & 0x7F);

        raw = _spi.transfer(0);
        flags |= (raw & 0x80) >> 6;
        uint8_t minute = BcdToUint8(raw & 0x7F);

        raw = _spi.transfer(0);
        flags |= (raw & 0x80) >> 5;
        uint8_t hour = BcdToBin24Hour(raw & 0x7f);

        raw = _spi.transfer(0);
        flags |= (raw & 0xc0) >> 3;
        uint8_t dayOf = BcdToUint8(raw & 0x3f);

        UnselectChip();
        _spi.endTransaction();

        if (flags == DS3234AlarmOneControl_HoursMinutesSecondsDayOfWeekMatch)
        {
            dayOf = RtcDateTime::ConvertRtcToDow(dayOf);
        }

        return DS3234AlarmOne(dayOf, hour, minute, second, (DS3234AlarmOneControl)flags);
    }

    DS3234AlarmTwo GetAlarmTwo()
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();

        _spi.transfer(DS3234_REG_ALARMTWO);

        uint8_t raw = _spi.transfer(0);
        uint8_t flags = (raw & 0x80) >> 7;
        uint8_t minute = BcdToUint8(raw & 0x7F);

        raw = _spi.transfer(0);
        flags |= (raw & 0x80) >> 6;
        uint8_t hour = BcdToBin24Hour(raw & 0x7f);

        raw = _spi.transfer(0);
        flags |= (raw & 0xc0) >> 4;
        uint8_t dayOf = BcdToUint8(raw & 0x3f);

        UnselectChip();
        _spi.endTransaction();

        if (flags == DS3234AlarmTwoControl_HoursMinutesDayOfWeekMatch)
        {
            dayOf = RtcDateTime::ConvertRtcToDow(dayOf);
        }

        return DS3234AlarmTwo(dayOf, hour, minute, (DS3234AlarmTwoControl)flags);
    }

    // Latch must be called after an alarm otherwise it will not
    // trigger again
    DS3234AlarmFlag LatchAlarmsTriggeredFlags()
    {
        uint8_t sreg = getReg(DS3234_REG_STATUS);
        uint8_t alarmFlags = (sreg & DS3234_AIFMASK);
        sreg &= ~DS3234_AIFMASK; // clear the flags
        setReg(DS3234_REG_STATUS, sreg);
        return (DS3234AlarmFlag)alarmFlags;
    }

    void SetTemperatureCompensationRate(DS3234TempCompensationRate rate)
    {
        uint8_t sreg = getReg(DS3234_REG_STATUS);

        sreg &= ~DS3234_CRATEMASK;
        sreg |= (rate << DS3234_CRATE0);

        setReg(DS3234_REG_STATUS, sreg);
    }

    DS3234TempCompensationRate GetTemperatureCompensationRate()
    {
        uint8_t sreg = getReg(DS3234_REG_STATUS);
        return (sreg & DS3234_CRATEMASK) >> DS3234_CRATE0;
    }

    void ForceTemperatureCompensationUpdate(bool block)
    {
        uint8_t creg = getReg(DS3234_REG_CONTROL);
        creg |= _BV(DS3234_CONV); // Write CONV bit
        setReg(DS3234_REG_CONTROL, creg);

        while (block && (creg & _BV(DS3234_CONV)) != 0)
        {
            // Block until CONV is 0
            creg = getReg(DS3234_REG_CONTROL);
        }
    }

    int8_t GetAgingOffset()
    {
        return getReg(DS3234_REG_AGING);
    }

    void SetAgingOffset(int8_t value)
    {
        setReg(DS3234_REG_AGING, value);
    }

    void SetMemory(uint8_t memoryAddress, uint8_t value)
    {
        SetMemory(memoryAddress, &value, 1);
    }

    uint8_t GetMemory(uint8_t memoryAddress)
    {
        uint8_t value;

        GetMemory(memoryAddress, &value, 1);

        return value;
    }

    uint8_t SetMemory(uint8_t memoryAddress, const uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t countWritten = 0;

        setReg(DS3234_REG_RAM_ADDRESS, memoryAddress);

        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();
        _spi.transfer(DS3234_REG_RAM_DATA | DS3234_REG_WRITE_FLAG);

        while (countBytes > 0)
        {
            _spi.transfer(*pValue++);
            
            countBytes--;
            countWritten++;
        }

        UnselectChip();
        _spi.endTransaction();

        return countWritten;
    }

    uint8_t GetMemory(uint8_t memoryAddress, uint8_t* pValue, uint8_t countBytes)
    {
        // set address to read from
        setReg(DS3234_REG_RAM_ADDRESS, memoryAddress);

        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();
        _spi.transfer(DS3234_REG_RAM_DATA);

        // read the data
        uint8_t countRead = 0;

        while (countBytes-- > 0)
        {
            *pValue++ = _spi.transfer(0);
            countRead++;
        }

        UnselectChip();
        _spi.endTransaction();

        return countRead;
    }

private:
    T_SPI_METHOD& _spi;
    uint8_t _csPin;

    void SelectChip()
    {
        digitalWrite(_csPin, LOW);
    }
    void UnselectChip()
    {
        digitalWrite(_csPin, HIGH);
    }

    uint8_t getReg(uint8_t regAddress)
    {
        uint8_t regValue;
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();
        _spi.transfer(regAddress);
        regValue = _spi.transfer(0);
        UnselectChip();
        _spi.endTransaction();

        return regValue;
    }

    void setReg(uint8_t regAddress, uint8_t regValue)
    {
        _spi.beginTransaction(c_Ds3234SpiSettings);
        SelectChip();
        _spi.transfer(regAddress | DS3234_REG_WRITE_FLAG);
        _spi.transfer(regValue);
        UnselectChip();
        _spi.endTransaction();
    }

};

#endif // __RTCDS3234_H__
