

#ifndef __RTCTEMPERATURE_H__
#define __RTCTEMPERATURE_H__


class RtcTemperature
{
public:
    RtcTemperature( int8_t msdegC, uint8_t lsdegC ) :
        mstemp( msdegC ),
        lstemp( lsdegC & 0xC0 )  // Bind to RTC resolution
    {
    }

    // Scaled integer temperature (x 256)
    int16_t AsScaledDegrees()
    {
        return(  (mstemp << 8) + lstemp  );  // Concatentate raw temp. regs 
    }

    // Rounded integer temperature (x 1)
    int8_t AsRoundedDegrees()
    {
        // Equivalent float function:
        //   SF ( sign(x)    * floor(  abs(       x ) + 0.5    ) ) / SF, with
        // =    ( sign(SF x) * floor(  abs(    SF x ) + 0.5 SF ) ) / SF  SF = 256
        // =      sign(x)    * floor( sign(x)( SF x ) + 0.5 SF )   / SF
        //
        int8_t  sgnT = ( (mstemp < 0) ? -1 : 1 );
        return( sgnT * ((sgnT * AsScaledDegrees() + 0x80) >> 8) );
    }

    // Float temperature (x 1)
    float AsFloat()
    {
        return ( (float)AsScaledDegrees() / 256.0f );
    }

    // Integer portion, temperature (x 1), useful for printing
    // Useful for printing
    int8_t AsWholeDegrees()
    {
        int8_t  sgnT = ( (mstemp < 0) ? -1 : 1 );
        return( sgnT * ((sgnT * AsScaledDegrees()) >> 8) ); // (x 1)
    }

    // Fractional portion, temperature (x100), useful for printing
    // Returns (0, 25, 50, or 75)
    uint8_t GetFractional()
    {
        int8_t  sgnT = ( (mstemp < 0) ? -1 : 1 );
        return(   25 * ((uint8_t)(sgnT * lstemp) >> 6)  );  // (x 100)
    }

protected:
    int8_t   mstemp;
    uint8_t  lstemp;
};

#endif // __RTCTEMPERATURE_H__
