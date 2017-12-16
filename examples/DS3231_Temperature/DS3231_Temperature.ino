
// CONNECTIONS:
// DS3231 SDA --> SDA
// DS3231 SCL --> SCL
// DS3231 VCC --> 3.3v or 5v
// DS3231 GND --> GND

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

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


void setup () 
{
    Serial.begin(115200);
    while (!Serial) delay(250);    // Wait until Arduino Serial Monitor opens
    Serial.println( " " );
    
    Serial.println(F("DS3231 Misc. Temperature Tests"));

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.print( " " );
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
        // Common Cuases:
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

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
} //setup

// Temperature test loop
void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    // Force a temperature A/D read
    Rtc.ForceTemperatureCompensationUpdate( true );

    // Output temperature in various formats
    printTemps();    

    delay(100);
        
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

void printTemps( void )
{
    // Get the temperature data    
    RtcTemperature temp = Rtc.GetTemperature();   

    // Print:
    // a) Floating point temperature
    //    Selectively add spaces to create const. width field
    //    ( no float formating for 'printf' type functions )

    float  tf = temp.AsFloat();
    if ( abs(tf) < 10 ) Serial.print( " " );    
    if ( tf >= 0 )      Serial.print( " " );
    Serial.print( tf, 2 );
    Serial.print( "   " );

    // Print:
    // b) Whole degrees/fractional portions
    //    formatted as a float
    // c) Rounded integer
    // d) Scaled integer
    //
    // 'snprintf' has no problems with integers

    char tstr[50];
    snprintf_P(tstr, 
            countof(tstr),
            PSTR("%3d.%02u  %3d   degC   Scaled:%6d" ),
            temp.AsWholeDegrees(),       // (b)
            temp.GetFractional(),
            temp.AsRoundedDegrees(),     // (c)
            temp.AsScaledDegrees()   );  // (d)
            
    Serial.println( tstr );
    
} //printTemp

