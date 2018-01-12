
// CONNECTIONS:
// DS3231 SDA --> SDA
// DS3231 SCL --> SCL
// DS3231 VCC --> 3.3v or 5v
// DS3231 GND --> GND

/* for software wire use below
#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

SoftwareWire myWire(SDA, SCL);
RtcDS3231<SoftwareWire> Rtc(myWire);
 for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
/* for normal hardware wire use above */

// Global used for printing
RtcTemperature  test( 0, 0 );

void setup () 
{
    Serial.begin(57600);
    while (!Serial) delay(250);   // Wait until Arduino Serial Monitor opens

    Serial.println( F(" ") );
    Serial.println(F("DS3231 Misc. Temperature Tests"));

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.print( "_" );          // Underscore to distinguish from 'printDateTime()' output
    Serial.println(__TIME__);

    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");

        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue

        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
    Serial.println();

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

    // Test RtcTemperature class methods across
    // negative -> zero -> positive temperature boundary

    Serial.println( F( "(-)->Zero->(+) Temperature Boundary Test" ) );
    for( float stf = -2; stf <= 2; stf += 0.25 )
    {
       // Find concatentated temperature regs
       int16_t ctreg = ( 256.0f * stf );

       // Split into separate regs & call constructor
       test = RtcTemperature( (ctreg >> 8), (ctreg & 0xFF) ); 

       // Show usage/results of RtcTemperature methods
       printTemps( test );
    }
    Serial.println();
     
} //setup

// Temperature test loop
void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    // Force a temperature A/D conversion to show how its done.
    // Temperature updates occur automatically every 64 sec in
    // normal operation, so forcing a conversion is not required.
    
    Rtc.ForceTemperatureCompensationUpdate( true );

    // Output temperature in various formats
    test = Rtc.GetTemperature();
    printTemps( test );    

    // Set update delay short enough to see changes of interest
    delay(100);   // Fast enough for part chilled with freeze spray
        
} //loop

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
    
} //printDateTime

// Examples of how to use methods in class RtcTemperature
void printTemps ( RtcTemperature& td )
{
    // Print:
    // a) Floating point temperature
    //    Selectively add spaces to create const. width field
    //    using 'Serial.print()' method
    //    ( no float formating for 'printf' type functions )

    float  tf = td.AsFloat();
    if ( abs(tf) < 10 ) Serial.print( " " );    
    if ( tf >= 0 )      Serial.print( " " );
    Serial.print( tf, 2 );
    Serial.print( " " );

    // Print:
    // b) Whole degrees/fractional portions formatted as a float
    //     i) 'printf' has no problems with integers
    //    ii) Special test needed -0.75 degC through -0.25 degC

    Serial.printf(
       // Distinguish between:        -0.xx   or   +0.xx       
       ( td.IsMinusSignNeeded() ? " -%1d.%02u" : "%3d.%02u" ),
       
       // These methods almost always used as a pair
       td.AsWholeDegrees(),    // Returns -128 through 127
       td.GetFractional()      // ....... 0, 25, 50, or 75
       
    ); //end, 'Serial.printf'

    Serial.printf( "%s", " " );     // Column spaces
  
    // Print using printf():
    // c) Rounded integer

    Serial.printf( "%3d   degC", td.AsRoundedDegrees() );
    Serial.printf( "%s", "    " );  // Column spaces

    // Print using printf():
    // d) Scaled integer, in
    //     i) Decimal:     (256 x degC)
    //    ii) Hexadecimal: DS3221 concatenated temperature
    //                     registers, <R11H:R12H> 
     
    int16_t  fxptDeg = td.AsFixedPointDegrees();
    Serial.printf( "Decimal/Hex: %5d / 0x%04X",    // 'X' - hex all caps
                   fxptDeg, (uint16_t)fxptDeg   );

    Serial.printf( "\n" ); // Newline char        

} //printTemps

