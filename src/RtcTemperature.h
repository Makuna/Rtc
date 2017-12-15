

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

//  Scaled integer temperature (x 256)
    int16_t AsScaledDegrees()
    {
        return(  (mstemp << 8) + lstemp  );
    }

    // Rounded integer temperature (x 1)
    int8_t AsRoundedDegrees()
    {
        // Equivalent to:
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

    // Deprecated, included for backwards compatibility
    int8_t AsWholeDegrees()
    {
        int8_t  sgnT = ( (mstemp < 0) ? -1 : 1 );
        return( sgnT * ((sgnT * AsScaledDegrees()) >> 8) ); // (x 1)
    }

    // Deprecated, included for backwards compatibility
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