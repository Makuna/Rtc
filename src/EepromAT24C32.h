#pragma once

//I2C Slave Address  
const uint8_t AT24C32_ADDRESS = 0x50; // 0b0 1010 A2 A1 A0

template<class T_WIRE_METHOD> class EepromAt24c32
{
public:
    EepromAt24c32(T_WIRE_METHOD& wire, uint8_t addressBits = 0b111) :
        _address(AT24C32_ADDRESS | (addressBits & 0b00000111)),
        _wire(wire),
        _lastError(0)
    {
    }

    void Begin()
    {
        _wire.begin();
    }

    uint8_t LastError()
    {
        return _lastError;
    }

    void SetMemory(uint16_t memoryAddress, uint8_t value)
    {
        SetMemory(memoryAddress, &value, 1);
    }

    uint8_t GetMemory(uint16_t memoryAddress)
    {
        uint8_t value;

        GetMemory(memoryAddress, &value, 1);
       
        return value;
    }

    // note: this method will write within a single page of eeprom.
    // Pages are 32 bytes (5 bits), so writing past a page boundary will
    // just wrap within the page of the starting memory address.  
    // 
    // xxxppppp pppaaaaa => p = page #, a = address within the page
    //
    // NOTE: hardware WIRE libraries often have a limit of a 32 byte send buffer.  The 
    // effect of this is that only 30 bytes can be sent, 2 bytes for the address to write to,
    // and then 30 bytes of the actual data. 
    uint8_t SetMemory(uint16_t memoryAddress, const uint8_t* pValue, uint8_t countBytes)
    {
        uint8_t countWritten = 0;

        beginTransmission(memoryAddress);

        while (countBytes > 0)
        {
            _wire.write(*pValue++);
            delay(10); // per spec, memory writes

            countBytes--;
            countWritten++;
        }

        _lastError = _wire.endTransmission();
        
        return countWritten;
    }

    // reading data does not wrap within pages, but due to only using
    // 12 (32K) or 13 (64K) bits are used, they will wrap within the memory limits
    // of the installed EEPROM
    //
    // NOTE: hardware WIRE libraries may have a limit of a 32 byte recieve buffer.  The 
    // effect of this is that only 32 bytes can be read at one time.
    uint8_t GetMemory(uint16_t memoryAddress, uint8_t* pValue, uint8_t countBytes)
    {
        // set address to read from
        beginTransmission(memoryAddress);
        _lastError = _wire.endTransmission();

        if (_lastError != 0)
        {
            return 0;
        }

        // read the data
        uint8_t countRead = 0;

        countRead = _wire.requestFrom(_address, countBytes);
        countBytes = countRead;

        while (countBytes-- > 0)
        {
            *pValue++ = _wire.read();
        }

        return countRead;
    }

private:
    const uint8_t _address;
    
    T_WIRE_METHOD& _wire;
    uint8_t _lastError;
    
    void beginTransmission(uint16_t memoryAddress)
    {
        _wire.beginTransmission(_address);
        _wire.write(memoryAddress >> 8);
        _wire.write(memoryAddress & 0xFf);
    }
};