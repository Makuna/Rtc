


#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <Wire.h>
#include "RtcUtility.h"
#include "RtcDS1307.h"

#define Write(x) write(static_cast<uint8_t>(x))
#define Read(x) read(x)

//I2C Slave Address  
#define DS1307_ADDRESS 0x68  

//DS1307 Register Addresses
#define DS1307_REG_TIMEDATE   0x00
#define DS1307_REG_STATUS     0x00
#define DS1307_REG_CONTROL    0x07
#define DS1307_REG_RAMSTART   0x08
#define DS1307_REG_RAMEND     0x3f

// DS1307 Control Register Bits
#define DS1307_RS0   0
#define DS1307_RS1   1
#define DS1307_SQWE  4
#define DS1307_OUT   7

// DS1307 Status Register Bits
#define DS1307_CH       7

void RtcDS1307::Begin()
{
    Wire.begin();
}

bool RtcDS1307::GetIsRunning()
{
    uint8_t sreg = getReg(DS1307_REG_STATUS);
    return !(sreg & _BV(DS1307_CH));
}

void RtcDS1307::SetIsRunning(bool isRunning)
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

void RtcDS1307::SetDateTime(const RtcDateTime& dt)
{
    // retain running state
    uint8_t sreg = getReg(DS1307_REG_STATUS) & _BV(DS1307_CH);

	// set the date time
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.Write(DS1307_REG_TIMEDATE);           

    Wire.Write(Uint8ToBcd(dt.Second()) | sreg);
    Wire.Write(Uint8ToBcd(dt.Minute()));
    Wire.Write(Uint8ToBcd(dt.Hour())); // 24 hour mode only

    Wire.Write(Uint8ToBcd(dt.DayOfWeek()));
    Wire.Write(Uint8ToBcd(dt.Day()));
    Wire.Write(Uint8ToBcd(dt.Month()));
    Wire.Write(Uint8ToBcd(dt.Year() - 2000));

    Wire.endTransmission();
}

RtcDateTime RtcDS1307::GetDateTime()
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.Write(DS1307_REG_TIMEDATE);           
    Wire.endTransmission();

    Wire.requestFrom(DS1307_ADDRESS, 7);
    uint8_t second = BcdToUint8(Wire.Read() & 0x7F);
    uint8_t minute = BcdToUint8(Wire.Read());
    uint8_t hour = BcdToBin24Hour(Wire.Read());

    Wire.Read();  // throwing away day of week as we calculate it

    uint8_t dayOfMonth = BcdToUint8(Wire.Read());
    uint8_t month = BcdToUint8(Wire.Read());
    uint16_t year = BcdToUint8(Wire.Read()) + 2000;

    return RtcDateTime(year, month, dayOfMonth, hour, minute, second);
}

void RtcDS1307::SetMemory(uint8_t memoryAddress, uint8_t value)
{
    uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
    if (address <= DS1307_REG_RAMEND)
    {
        setReg(address, value);
    }
}

uint8_t RtcDS1307::GetMemory(uint8_t memoryAddress)
{
    uint8_t value = 0;
    uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
    if (address <= DS1307_REG_RAMEND)
    {
        value = getReg(address);
    }
    return value;
}

void RtcDS1307::SetSquareWavePin(DS1307SquareWaveOut pinMode)
{
    setReg(DS1307_REG_CONTROL, pinMode);
}

uint8_t RtcDS1307::SetMemory(uint8_t memoryAddress, const uint8_t* pValue, uint8_t countBytes)
{
    uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
    uint8_t countWritten = 0;
    if (address <= DS1307_REG_RAMEND)
    {
        Wire.beginTransmission(DS1307_ADDRESS);
        Wire.Write(address);

        while (countBytes > 0 && address <= DS1307_REG_RAMEND)
        {
            Wire.Write(*pValue++);
            address++;
            countBytes--;
            countWritten++;
        }

        Wire.endTransmission();
    }
    return countWritten;
}

uint8_t RtcDS1307::GetMemory(uint8_t memoryAddress, uint8_t* pValue, uint8_t countBytes)
{
    uint8_t address = memoryAddress + DS1307_REG_RAMSTART;
    uint8_t countRead = 0;
    if (address <= DS1307_REG_RAMEND)
    {
        countBytes = min(countBytes, DS1307_REG_RAMEND - DS1307_REG_RAMSTART);
        Wire.beginTransmission(DS1307_ADDRESS);
        Wire.Write(address);
        Wire.endTransmission();

        Wire.requestFrom(DS1307_ADDRESS, (int)countBytes);

        while (countBytes-- > 0)
        {
            *pValue++ = Wire.Read();
            countRead++;
        }
    }

    return countRead;
}

uint8_t RtcDS1307::getReg(uint8_t regAddress)
{
    Wire.beginTransmission(DS1307_ADDRESS);
	Wire.Write(regAddress);
	Wire.endTransmission();

	// control register
	Wire.requestFrom(DS1307_ADDRESS, 1);

	uint8_t regValue = Wire.Read();     
    return regValue;
}

void RtcDS1307::setReg(uint8_t regAddress, uint8_t regValue)
{
    Wire.beginTransmission(DS1307_ADDRESS);
	Wire.Write(regAddress);
	Wire.Write(regValue);
	Wire.endTransmission();
}
