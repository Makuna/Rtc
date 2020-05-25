

#ifndef __RTCDS1307_H__
#define __RTCDS1307_H__

#include <Arduino.h>
#include "RtcDateTime.h"
#include "RtcUtility.h"

//I2C Slave Address  
const uint8_t DS1307_ADDRESS = 0x68;

//DS1307 Register Addresses
const uint8_t DS1307_REG_TIMEDATE   = 0x00;
const uint8_t DS1307_REG_STATUS     = 0x00;
const uint8_t DS1307_REG_CONTROL    = 0x07;
const uint8_t DS1307_REG_RAMSTART   = 0x08;
const uint8_t DS1307_REG_RAMEND     = 0x3f;
const uint8_t DS1307_REG_RAMSIZE = DS1307_REG_RAMEND - DS1307_REG_RAMSTART;

//DS1307 Register Data Size if not just 1
const uint8_t DS1307_REG_TIMEDATE_SIZE = 7;

// DS1307 Control Register Bits
const uint8_t DS1307_RS0   = 0;
const uint8_t DS1307_RS1   = 1;
const uint8_t DS1307_SQWE  = 4;
const uint8_t DS1307_OUT   = 7;

// DS1307 Status Register Bits
const uint8_t DS1307_CH       = 7;

enum DS1307SquareWaveOut
{
    DS1307SquareWaveOut_1Hz  =  0b00010000,
    DS1307SquareWaveOut_4kHz =  0b00010001,
    DS1307SquareWaveOut_8kHz =  0b00010010,
    DS1307SquareWaveOut_32kHz = 0b00010011,
    DS1307SquareWaveOut_High =  0b10000000,
    DS1307SquareWaveOut_Low =   0b00000000,
};

template<class T_WIRE_METHOD> class RtcDS1307
{
public:
    RtcDS1307(T_WIRE_METHOD& wire) :
        _wire(wire),
        _lastError(0)
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
        return GetIsRunning();
    }

    bool GetIsRunning()
    {
        uint8_t sreg = getReg(DS1307_REG_STATUS);
        return (!(sreg & _BV(DS1307_CH)) && (_lastError == 0));
    }

    void SetIsRunning(bool isRunning)
    {
        uint8_t sreg = getReg(DS1307_REG_STATUS);
        if (isRunning)
        {
            sreg &= ~_BV(DS1307_CH);
        }
        else
        {
            sreg |= _BV(DS1307_CH);
        }
        setReg(DS1307_REG_STATUS, sreg);
    }

    void SetDateTime(const RtcDateTime& dt)
    {
        // retain running state
        uint8_t sreg = getReg(DS1307_REG_STATUS) & _BV(DS1307_CH);

        // set the date time
        _wire.beginTransmission(DS1307_ADDRESS);
        _wire.write(DS1307_REG_TIMEDATE);

        _wire.write(Uint8ToBcd(dt.Second()) | sreg);
        _wire.write(Uint8ToBcd(dt.Minute()));
        _wire.write(Uint8ToBcd(dt.Hour())); // 24 hour mode only

        // RTC Hardware Day of Week is 1-7, 1 = Monday
        // convert our Day of Week to Rtc Day of Week
        uint8_t rtcDow = RtcDateTime::ConvertDowToRtc(dt.DayOfWeek());

        _wire.write(Uint8ToBcd(rtcDow)); 
        _wire.write(Uint8ToBcd(dt.Day()));
        _wire.write(Uint8ToBcd(dt.Month()));
        _wire.write(Uint8ToBcd(dt.Year() - 2000));

        _lastError = _wire.endTransmission();
    }

    RtcDateTime GetDateTime()
    {
        _wire.beginTransmission(DS1307_ADDRESS);
        _wire.write(DS1307_REG_TIMEDATE);
        _lastError = _wire.endTransmission();
        if (_lastError != 0)
        {
            RtcDateTime(0);
        }

        uint8_t bytesRead = _wire.requestFrom(DS1307_ADDRESS, DS1307_REG_TIMEDATE_SIZE);
        if (DS1307_REG_TIMEDATE_SIZE != bytesRead)
        {
            _lastError = 4;
            RtcDateTime(0);
        }

        uint8_t second = BcdToUint8(_wire.read() & 0x7F);
        uint8_t minute = BcdToUint8(_wire.read());
        uint8_t hour = BcdToBin24Hour(_wire.read());

        _wire.read();  // throwing away day of week as we calculate it

        uint8_t dayOfMonth = BcdToUint8(_wire.read());
        uint8_t month = BcdToUint8(_wire.read());
        uint16_t year = BcdToUint8(_wire.read()) + 2000;

        return RtcDateTime(year, month, dayOfMonth, hour, minute, second);
    }

    void SetMemory(uint8_t memoryAddress, uint8_t value)
    {
        uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
        if (address <= DS1307_REG_RAMEND)
        {
            setReg(address, value);
        }
    }

    uint8_t GetMemory(uint8_t memoryAddress)
    {
        uint8_t value = 0;
        uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
        if (address <= DS1307_REG_RAMEND)
        {
            value = getReg(address);
        }
        return value;
    }

    uint8_t SetMemory(uint8_t memoryAddress, const uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
        uint8_t countWritten = 0;
        if (address <= DS1307_REG_RAMEND)
        {
            _wire.beginTransmission(DS1307_ADDRESS);
            _wire.write(address);

            while (countBytes > 0 && address <= DS1307_REG_RAMEND)
            {
                _wire.write(*pValue++);
                address++;
                countBytes--;
                countWritten++;
            }

            _lastError = _wire.endTransmission();
        }
        return countWritten;
    }

    uint8_t GetMemory(uint8_t memoryAddress, uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
        uint8_t countRead = 0;
        if (address <= DS1307_REG_RAMEND)
        {
            if (countBytes > DS1307_REG_RAMSIZE)
            {
                countBytes = DS1307_REG_RAMSIZE;
            }

            _wire.beginTransmission(DS1307_ADDRESS);
            _wire.write(address);
            _lastError = _wire.endTransmission();
            if (_lastError != 0)
            {
                return 0;
            }

            countRead = _wire.requestFrom(DS1307_ADDRESS, countBytes);
            countBytes = countRead;

            while (countBytes-- > 0)
            {
                *pValue++ = _wire.read();
            }
        }

        return countRead;
    }

    void SetSquareWavePin(DS1307SquareWaveOut pinMode)
    {
        setReg(DS1307_REG_CONTROL, pinMode);
    }

private:
    T_WIRE_METHOD& _wire;
    uint8_t _lastError;

    uint8_t getReg(uint8_t regAddress)
    {
        _wire.beginTransmission(DS1307_ADDRESS);
        _wire.write(regAddress);
        _lastError = _wire.endTransmission();
        if (_lastError != 0)
        {
            return 0;
        }

        // control register
        uint8_t bytesRead = _wire.requestFrom(DS1307_ADDRESS, (uint8_t)1);
        if (1 != bytesRead)
        {
            _lastError = 4;
            return 0;
        }

        uint8_t regValue = _wire.read();
        return regValue;
    }

    void setReg(uint8_t regAddress, uint8_t regValue)
    {
        _wire.beginTransmission(DS1307_ADDRESS);
        _wire.write(regAddress);
        _wire.write(regValue);
        _lastError = _wire.endTransmission();
    }
};

#endif // __RTCDS1307_H__