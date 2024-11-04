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
#include "ThreeWire.h"


//DS1302 Register Addresses
const uint8_t DS1302_REG_TIMEDATE   = 0x80;
const uint8_t DS1302_REG_TIMEDATE_BURST = 0xBE;
const uint8_t DS1302_REG_TCR    = 0x90;
const uint8_t DS1302_REG_RAM_BURST = 0xFE;
const uint8_t DS1302_REG_RAMSTART   = 0xc0;
const uint8_t DS1302_REG_RAMEND     = 0xfd;
// ram read and write addresses are interleaved
const uint8_t DS1302RamSize = 31;


// DS1302 Trickle Charge Control Register Bits
enum DS1302TcrResistor : uint8_t
{
    DS1302TcrResistor_Disabled = 0,
    DS1302TcrResistor_2KOhm = 0b00000001,
    DS1302TcrResistor_4KOhm = 0b00000010,
    DS1302TcrResistor_8KOhm = 0b00000011,
    DS1302TcrResistor_MASK  = 0b00000011,
};

enum DS1302TcrDiodes : uint8_t
{
    DS1302TcrDiodes_None = 0,
    DS1302TcrDiodes_One      = 0b00000100,
    DS1302TcrDiodes_Two      = 0b00001000,
    DS1302TcrDiodes_Disabled = 0b00001100,
    DS1302TcrDiodes_MASK     = 0b00001100,
};

enum DS1302TcrStatus : uint8_t
{
    DS1302TcrStatus_Enabled  = 0b10100000,
    DS1302TcrStatus_Disabled = 0b01010000,
    DS1302TcrStatus_MASK     = 0b11110000,
};

const uint8_t DS1302Tcr_Disabled = static_cast<uint8_t>(DS1302TcrStatus_Disabled) | 
                                   static_cast<uint8_t>(DS1302TcrDiodes_Disabled) | 
                                   static_cast<uint8_t>(DS1302TcrResistor_Disabled);

// DS1302 Clock Halt Register & Bits
const uint8_t DS1302_REG_CH = 0x80; // bit in the seconds register
const uint8_t DS1302_CH     = 7;

// Write Protect Register & Bits
const uint8_t DS1302_REG_WP = 0x8E; 
const uint8_t DS1302_WP = 7;

template<class T_WIRE_METHOD> class RtcDS1302
{
public:
    RtcDS1302(T_WIRE_METHOD& wire) :
        _wire(wire)
    {
    }

    void Begin()
    {
        _wire.begin();
    }

    bool GetIsWriteProtected()
    {
        uint8_t wp = getReg(DS1302_REG_WP);
        return !!(wp & _BV(DS1302_WP));
    }

    void SetIsWriteProtected(bool isWriteProtected)
    {
        uint8_t wp = getReg(DS1302_REG_WP);
        if (isWriteProtected)
        {
            wp |= _BV(DS1302_WP);
        }
        else
        {
            wp &= ~_BV(DS1302_WP);
        }
        setReg(DS1302_REG_WP, wp);
    }

    bool IsDateTimeValid()
    {
        return GetDateTime().IsValid();
    }

    bool GetIsRunning()
    {
        uint8_t ch = getReg(DS1302_REG_CH);
        return !(ch & _BV(DS1302_CH));
    }

    void SetIsRunning(bool isRunning)
    {
        uint8_t ch = getReg(DS1302_REG_CH);
        if (isRunning)
        {
            ch &= ~_BV(DS1302_CH);
        }
        else
        {
            ch |= _BV(DS1302_CH);
        }
        setReg(DS1302_REG_CH, ch);
    }

    uint8_t GetTrickleChargeSettings()
    {
        uint8_t setting = getReg(DS1302_REG_TCR);
        return setting;
    }

    void SetTrickleChargeSettings(uint8_t setting)
    {
        if ((setting & DS1302TcrResistor_MASK) == DS1302TcrResistor_Disabled) 
        {
            // invalid resistor setting, set to disabled
            setting = DS1302Tcr_Disabled;
        }
        else if ((setting & DS1302TcrDiodes_MASK) == DS1302TcrDiodes_Disabled ||
            (setting & DS1302TcrDiodes_MASK) == DS1302TcrDiodes_None) 
        {
            // invalid diode setting, set to disabled
            setting = DS1302Tcr_Disabled;
        }
        else if ((setting & DS1302TcrStatus_MASK) != DS1302TcrStatus_Enabled) 
        {
            // invalid status setting, set to disabled
            setting = DS1302Tcr_Disabled;
        }

        setReg(DS1302_REG_TCR, setting);
    }

    void SetDateTime(const RtcDateTime& dt)
    {
        // set the date time
        _wire.beginTransmission(DS1302_REG_TIMEDATE_BURST);

        _wire.write(Uint8ToBcd(dt.Second()));
        _wire.write(Uint8ToBcd(dt.Minute()));
        _wire.write(Uint8ToBcd(dt.Hour())); // 24 hour mode only
        _wire.write(Uint8ToBcd(dt.Day()));
        _wire.write(Uint8ToBcd(dt.Month()));

        // RTC Hardware Day of Week is 1-7, 1 = Monday
        // convert our Day of Week to Rtc Day of Week
        uint8_t rtcDow = RtcDateTime::ConvertDowToRtc(dt.DayOfWeek());

        _wire.write(Uint8ToBcd(rtcDow));
        _wire.write(Uint8ToBcd(dt.Year() - 2000));
        _wire.write(0); // no write protect, as all of this is ignored if it is protected

        _wire.endTransmission();
    }

    RtcDateTime GetDateTime()
    {
        _wire.beginTransmission(DS1302_REG_TIMEDATE_BURST | THREEWIRE_READFLAG);

        uint8_t second = BcdToUint8(_wire.read() & 0x7F);
        uint8_t minute = BcdToUint8(_wire.read());
        uint8_t hour = BcdToBin24Hour(_wire.read());
        uint8_t dayOfMonth = BcdToUint8(_wire.read());
        uint8_t month = BcdToUint8(_wire.read());

        _wire.read();  // throwing away day of week as we calculate it

        uint16_t year = BcdToUint8(_wire.read()) + 2000;

        _wire.read();  // throwing away write protect flag

        _wire.endTransmission();

        return RtcDateTime(year, month, dayOfMonth, hour, minute, second);
    }

    void SetMemory(uint8_t memoryAddress, uint8_t value)
    {
        // memory addresses interleaved read and write addresses
        // so we need to calculate the offset
        uint8_t address = memoryAddress * 2 + DS1302_REG_RAMSTART;
        if (address <= DS1302_REG_RAMEND)
        {
            setReg(address, value);
        }
    }

    uint8_t GetMemory(uint8_t memoryAddress)
    {
        uint8_t value = 0;
        // memory addresses interleaved read and write addresses
        // so we need to calculate the offset
        uint8_t address = memoryAddress * 2 + DS1302_REG_RAMSTART;
        if (address <= DS1302_REG_RAMEND)
        {
            value = getReg(address);
        }
        return value;
    }

    uint8_t SetMemory(const uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t countWritten = 0;

        _wire.beginTransmission(DS1302_REG_RAM_BURST);

        while (countBytes > 0 && countWritten < DS1302RamSize)
        {
            _wire.write(*pValue++);
            countBytes--;
            countWritten++;
        }

        _wire.endTransmission();

        return countWritten;
    }

    uint8_t GetMemory(uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t countRead = 0;

        _wire.beginTransmission(DS1302_REG_RAM_BURST | THREEWIRE_READFLAG);

        while (countBytes > 0 && countRead < DS1302RamSize)
        {
            *pValue++ = _wire.read();
            countRead++;
            countBytes--;
        }

        _wire.endTransmission();

        return countRead;
    }

private:
    T_WIRE_METHOD& _wire;

    uint8_t getReg(uint8_t regAddress)
    {
        _wire.beginTransmission(regAddress | THREEWIRE_READFLAG);
        uint8_t regValue = _wire.read();
        _wire.endTransmission();
        return regValue;
    }

    void setReg(uint8_t regAddress, uint8_t regValue)
    {
        _wire.beginTransmission(regAddress);
        _wire.write(regValue);
        _wire.endTransmission();
    }
};

