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

#include "RtcDS3231.h"

// DS3232 is the same as the DS3231 except it has SRAM
//
// 
//I2C Slave Address  
const uint8_t DS3232_ADDRESS = DS3231_ADDRESS;

const uint8_t DS3232_REG_SRAMFIRST = 0x14;
const uint8_t DS3232_REG_SRAMLAST = 0xff;

const uint8_t DS3232_REG_SRAMSIZE = DS3232_REG_SRAMLAST - DS3232_REG_SRAMFIRST + 1;

template<class T_WIRE_METHOD> class RtcDS3232 : 
    public RtcDS3231<T_WIRE_METHOD>
{
public:
    RtcDS3232(T_WIRE_METHOD& wire) :
        RtcDS3231<T_WIRE_METHOD>( wire )
    {
    }

    void SetMemory(uint8_t memoryAddress, uint8_t value)
    {
        uint8_t address = memoryAddress + DS3232_REG_SRAMFIRST;
        if (address <= DS3232_REG_SRAMLAST)
        {
            this->setReg(address, value);
        }
    }

    uint8_t GetMemory(uint8_t memoryAddress)
    {
        uint8_t value = 0;
        uint8_t address = memoryAddress + DS3232_REG_SRAMFIRST;
        if (address <= DS3232_REG_SRAMLAST)
        {
            value = this->getReg(address);
        }
        return value;
    }

    uint8_t SetMemory(uint8_t memoryAddress, const uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t address = memoryAddress + DS3232_REG_SRAMFIRST;
        uint8_t countWritten = 0;
        if (address <= DS3232_REG_SRAMLAST)
        {
            this->_wire.beginTransmission(DS3232_ADDRESS);
            this->_wire.write(address);

            while (countBytes > 0 && address <= DS3232_REG_SRAMLAST)
            {
                this->_wire.write(*pValue++);
                address++;
                countBytes--;
                countWritten++;
            }

            this->_lastError = this->_wire.endTransmission();
        }
        return countWritten;
    }

    size_t GetMemory(uint8_t memoryAddress, uint8_t* pValue, size_t countBytes)
    {
        uint8_t address = memoryAddress + DS3232_REG_SRAMFIRST;
        size_t countRead = 0;
        if (address <= DS3232_REG_SRAMLAST)
        {
            if (countBytes > DS3232_REG_SRAMSIZE)
            {
                countBytes = DS3232_REG_SRAMSIZE;
            }

            this->_wire.beginTransmission(DS3232_ADDRESS);
            this->_wire.write(address);
            this->_lastError = this->_wire.endTransmission();
            if (this->_lastError != Rtc_Wire_Error_None)
            {
                return 0;
            }

            countRead = this->_wire.requestFrom(DS3232_ADDRESS, countBytes);
            countBytes = countRead;

            while (countBytes-- > 0)
            {
                *pValue++ = this->_wire.read();
            }
        }

        return countRead;
    }

};