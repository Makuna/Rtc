
// CONNECTIONS:
// DS1307 SDA --> SDA
// DS1307 SCL --> SCL
// DS1307 VCC --> 5v
// DS1307 GND --> GND

#define countof(a) (sizeof(a) / sizeof(a[0]))

/* for software wire use below
#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
#include <EepromAt24C32.h>

SoftwareWire myWire(SDA, SCL);
RtcDS1307<SoftwareWire> Rtc(myWire);
/* for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
#include <EepromAt24C32.h>

RtcDS3231<TwoWire> Rtc(Wire);
EepromAt24c32<TwoWire> RtcEeprom(Wire);

// if you have any of the address pins on the RTC soldered together
// then you need to provide the state of those pins, normally they
// are connected to vcc with a reading of 1, if soldered they are 
// grounded with a reading of 0.  The bits are in the order A2 A1 A0
// thus the following would have the A2 soldered together
// EepromAt24c32<TwoWire> RtcEeprom(Wire, 0b011);

/* for normal hardware wire use above */

// nothing longer than 32 bytes
// rtc eeprom memory is 32 byte pages
// writing is limited to each page, so it will wrap at page
// boundaries. 
// But reading is only limited by the buffer in Wire class which
// by default is 32
const char data[] = "What time is it in Greenwich?";
const uint16_t stringAddr = 64; // stored on page boundary

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
    RtcEeprom.Begin();
    
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
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the Eeprom

    // store starting address of string
    RtcEeprom.SetMemory(0, stringAddr); 
    // store the string, nothing longer than 32 bytes due to paging
    uint8_t written = RtcEeprom.SetMemory(stringAddr, (const uint8_t*)data, sizeof(data) - 1); // remove the null terminator strings add
    // store the length of the string
    RtcEeprom.SetMemory(1, written); // store the 
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
    uint8_t address = RtcEeprom.GetMemory(0);
    if (address != stringAddr)
    {
        Serial.print("address didn't match ");
        Serial.println(address);
    }
    
    {
        // get the size of the data from address 1
        uint8_t count = RtcEeprom.GetMemory(1);
        uint8_t buff[64];

        // get our data from the address with the given size
        uint8_t gotten = RtcEeprom.GetMemory(address, buff, count);

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

