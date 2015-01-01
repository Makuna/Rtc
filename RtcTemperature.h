

#ifndef __RTCTEMPERATURE_H__
#define __RTCTEMPERATURE_H__


class RtcTemperature
{
public:
    RtcTemperature(int8_t degrees, uint8_t fraction) :
        integerDegrees(degrees),
        decimalFraction(fraction)
    {
    }

    float AsFloat()
    {
        float degrees = (float)integerDegrees;
        degrees += ((degrees < 0) ? -1 : 1) * (float)decimalFraction;
        return degrees;
    }

    int8_t AsWholeDegrees()
    {
        return integerDegrees;
    }

    uint8_t GetFractional()
    {
        return decimalFraction;
    }

protected:
    int8_t integerDegrees;
    uint8_t decimalFraction;
};

#endif // __RTCTEMPERATURE_H__