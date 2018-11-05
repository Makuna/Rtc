
// CONNECTIONS:
// DS3234 MISO --> MISO
// DS3234 MOSI --> MOSI
// DS3234 CLK  --> CLK (SCK)
// DS3234 CS (SS) --> CS (pin set aside specific to the DS3234 device using SPI)
// DS3234 VCC --> 3.3v or 5v
// DS3234 GND --> GND


#define countof(a) (sizeof(a) / sizeof(a[0]))

#include <RtcDS3234.h>

SoftwareWire myWire(SDA, SCL);
RtcDS3234<SoftwareWire> Rtc(myWire);
 for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3234.h>
RtcDS3234<TwoWire> Rtc(Wire);
/* for normal hardware wire use above */


const char data[] = "what time is it";

void setup () 
{
    Serial.begin(57600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
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
        Serial.println("RTC lost confidence in the DateTime!");
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

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.SetSquareWavePin(DS3234SquareWaveOut_Low); 

/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    Rtc.SetMemory(0, 13);
    uint8_t written = Rtc.SetMemory(13, (const uint8_t*)data, sizeof(data) - 1); // remove the null terminator strings add
    Rtc.SetMemory(1, written);
/* end of comment out section */
}

void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    delay(5000);

    // read data

    // get the offset we stored our data from address zero
    uint8_t address = Rtc.GetMemory(0);
    if (address != 13)
    {
        Serial.println("address didn't match");
    }
    else
    {
        // get the size of the data from address 1
        uint8_t count = Rtc.GetMemory(1);
        uint8_t buff[20];

        // get our data from the address with the given size
        uint8_t gotten = Rtc.GetMemory(address, buff, count);

        if (gotten != count ||
            count != sizeof(data) - 1) // remove the extra null terminator strings add
        {
            Serial.print("something didn't match, count = ");
            Serial.print(count, DEC);
            Serial.print(", gotten = ");
            Serial.print(gotten, DEC);
            Serial.println();
        }
        Serial.print("data read (");
        Serial.print(gotten);
        Serial.print(") = \"");
        for (uint8_t ch = 0; ch < gotten; ch++)
        {
            Serial.print((char)buff[ch]);
        }
        Serial.println("\"");
    }

    
    delay(5000);
}



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
}

