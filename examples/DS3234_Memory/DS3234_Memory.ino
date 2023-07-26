
// Reference for connecting SPI see https://www.arduino.cc/en/Reference/SPI
// CONNECTIONS:
// DS3234 MISO --> MISO
// DS3234 MOSI --> MOSI
// DS3234 CLK  --> CLK (SCK)
// DS3234 CS (SS) --> 5 (pin used to select the DS3234 on the SPI)
// DS3234 VCC --> 3.3v or 5v
// DS3234 GND --> GND

const uint8_t DS3234_CS_PIN = 5;

#include <SPI.h>
#include <RtcDS3234.h>

RtcDS3234<SPIClass> Rtc(SPI, DS3234_CS_PIN);

const char data[] = "what time is it";

void setup () 
{
    Serial.begin(115200);
    while (!Serial);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    
    SPI.begin();
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
    Rtc.SetSquareWavePin(DS3234SquareWavePin_ModeNone); 

/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    Rtc.SetMemory(0, 13); // address of a data item
    uint8_t written = Rtc.SetMemory(13, (const uint8_t*)data, sizeof(data) - 1); // remove the null terminator strings add
    Rtc.SetMemory(1, written); // size of data time 
/* end of comment out section */
}

void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
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

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];

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

