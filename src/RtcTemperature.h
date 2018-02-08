

#ifndef __RTCTEMPERATURE_H__
#define __RTCTEMPERATURE_H__


class RtcTemperature
{
public:
    // Constructor
    RtcTemperature ( int8_t ubDegC, uint8_t lbDegC )
    {
    // a) Merge RTC registers into signed scaled temperature (x256),
    //    then bind to RTC resolution.
    //         |         r11h          | DP |         r12h         |
    // Bit:     15 14 13 12 11 10  9  8   .  7  6  5  4  3  2  1  0  -1 -2
    //           s  i  i  i  i  i  i  i   .  f  f  0  0  0  0  0  0
    //
    // b) Rescale to (x4) by right-shifting (6) bits
    //         |                                         | DP |    |
    // Bit:     15 14 13 12 11 10  9  8  7  6  5  4  3  2   .  1  0  -1 -2
    //           s  s  s  s  s  s  s  i  i  i  i  i  i  i      f  f   0  0

       scaledDegC = (int16_t)( ( (ubDegC << 8) | (lbDegC & 0xC0) ) >> 6 );
    }

    // Float temperature (x 1)
    float AsFloatDegC()
    {
        return( (float)scaledDegC / 4.0f );
    }

protected:
    int16_t  scaledDegC;  // Scaled temperature (4 x degC)
};

#endif // __RTCTEMPERATURE_H__