

#ifndef __RTCDS1307_H__
#define __RTCDS1307_H__

#include <Arduino.h>
#include <RtcDateTime.h>

enum DS1307SquareWaveOut
{
    DS1307SquareWaveOut_1Hz  =  0b00010000,
    DS1307SquareWaveOut_4kHz =  0b00010001,
    DS1307SquareWaveOut_8kHz =  0b00010010,
    DS1307SquareWaveOut_32kHz = 0b00010011,
    DS1307SquareWaveOut_High =  0b10000000,
    DS1307SquareWaveOut_Low =   0b00000000,
};

class RtcDS1307
{
public:
    static void Begin();

    static bool IsDateTimeValid()
    {
        return GetIsRunning();
    }

    static bool GetIsRunning();	
    static void SetIsRunning(bool isRunning);

    static void SetDateTime(const RtcDateTime& dt);	
    static RtcDateTime GetDateTime();	

    static void SetMemory(uint8_t memoryAddress, uint8_t value);
    static uint8_t GetMemory(uint8_t memoryAddress);

    static uint8_t SetMemory(uint8_t memoryAddress, const uint8_t* pValue, uint8_t countBytes);
    static uint8_t GetMemory(uint8_t memoryAddress, uint8_t* pValue, uint8_t countBytes);

    static void SetSquareWavePin(DS1307SquareWaveOut pinMode);

private:
    static uint8_t getReg(uint8_t regAddress);
    static void setReg(uint8_t regAddress, uint8_t regValue);
};

#endif // __RTCDS1307_H__