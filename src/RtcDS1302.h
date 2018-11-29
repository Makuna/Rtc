

#ifndef __RTCDS1302_H__
#define __RTCDS1302_H__

#include <Arduino.h>
#include "RtcDateTime.h"
#include "RtcUtility.h"



//DS1302 Register Addresses
const uint8_t DS1302_REG_TIMEDATE   = 0x80;
const uint8_t DS1302_REG_TIMEDATE_BURST = 0xBE;
const uint8_t DS1302_REG_TCR    = 0x90;
const uint8_t DS1302_REG_RAM_BURST = 0xFE;
const uint8_t DS1302_REG_RAMSTART   = 0xc0;
const uint8_t DS1302_REG_RAMEND     = 0xfd;
// ram read and write addresses are interleaved
const uint8_t DS1302_REG_RAMSIZE = 31;


// DS1302 Trickle Charge Control Register Bits
enum DS1302_TCR_RESISTOR {
    DS1302_TCR_RESISTOR_DISABLED = 0,
    DS1302_TCR_RESISTOR_2KOHM = B00000001,
    DS1302_TCR_RESISTOR_4KOHM = B00000010,
    DS1302_TCR_RESISTOR_8KOHM = B00000011,
    DS1302_TCR_RESISTOR_MASK  = B00000011,
};

enum DS1302_TCR_DIODES {
    DS1302_TCR_DIODES_NONE = 0,
    DS1302_TCR_DIODES_ONE      = B00000100,
    DS1302_TCR_DIODES_TWO      = B00001000,
    DS1302_TCR_DIODES_DISABLED = B00001100,
    DS1302_TCR_DIODES_MASK     = B00001100,
};

enum DS1302_TCR_STATUS {
    DS1302_TCR_STATUS_ENABLED  = B10100000,
    DS1302_TCR_STATUS_DISABLED = B01010000,
    DS1302_TCR_STATUS_MASK     = B11110000,
};

const uint8_t DS1302_TCR_DISABLED = DS1302_TCR_STATUS_DISABLED | DS1302_TCR_DIODES_DISABLED | DS1302_TCR_RESISTOR_DISABLED;

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
        if ((setting & DS1302_TCR_RESISTOR_MASK) == DS1302_TCR_RESISTOR_DISABLED) {
            // invalid resistor setting, set to disabled
            setting = DS1302_TCR_DISABLED;
            goto apply;
        }
        if ((setting & DS1302_TCR_DIODES_MASK) == DS1302_TCR_DIODES_DISABLED ||
            (setting & DS1302_TCR_DIODES_MASK) == DS1302_TCR_DIODES_NONE) {
            // invalid diode setting, set to disabled
            setting = DS1302_TCR_DISABLED;
            goto apply;
        }
        if ((setting & DS1302_TCR_STATUS_MASK) != DS1302_TCR_STATUS_ENABLED) {
            // invalid status setting, set to disabled
            setting = DS1302_TCR_DISABLED;
            goto apply;
        }

     apply:
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

        while (countBytes > 0 && countWritten < DS1302_REG_RAMSIZE)
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

        while (countBytes > 0 && countRead < DS1302_REG_RAMSIZE)
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

#endif // __RTCDS1302_H__