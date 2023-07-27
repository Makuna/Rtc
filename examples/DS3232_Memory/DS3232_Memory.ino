
// CONNECTIONS:
// DS3232 SDA --> SDA
// DS3232 SCL --> SCL
// DS3232 VCC --> 5v
// DS3232 GND --> GND

#define countof(a) (sizeof(a) / sizeof(a[0]))

/* for software wire use below
#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3232.h>

SoftwareWire myWire(SDA, SCL);
RtcDS3232<SoftwareWire> Rtc(myWire);
 for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3232.h>
RtcDS3232<TwoWire> Rtc(Wire);
/* for normal hardware wire use above */


// nothing longer than 32 bytes
// rtc eeprom memory is 32 byte pages
// writing is limited to each page, so it will wrap at page
// boundaries. 
// But reading is only limited by the buffer in Wire class which
// by default is 32

// example settings objec that will be serialized into and out of EEPROM
struct Settings
{
    uint8_t size; // size of this structure for versioning/validation
    uint8_t value1; 
    uint16_t value2;
    uint32_t value3;
    float value4;
};

// where in EEPROM we will store the settings
const uint8_t SettingsAddress = 0;

// handy routine to return true if there was an error
// but it will also print out an error message with the given topic
bool wasError(const char* errorTopic = "")
{
    uint8_t error = Rtc.LastError();
    if (error != 0)
    {
        // we have a communications error
        // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
        // for what the number means
        Serial.print("[");
        Serial.print(errorTopic);
        Serial.print("] WIRE communications error (");
        Serial.print(error);
        Serial.print(") : ");

        switch (error)
        {
        case Rtc_Wire_Error_None:
            Serial.println("(none?!)");
            break;
        case Rtc_Wire_Error_TxBufferOverflow:
            Serial.println("transmit buffer overflow");
            break;
        case Rtc_Wire_Error_NoAddressableDevice:
            Serial.println("no device responded");
            break;
        case Rtc_Wire_Error_UnsupportedRequest:
            Serial.println("device doesn't support request");
            break;
        case Rtc_Wire_Error_Unspecific:
            Serial.println("unspecified error");
            break;
        case Rtc_Wire_Error_CommunicationTimeout:
            Serial.println("communications timed out");
            break;
        }
        return true;
    }
    return false;
}

void setup () 
{
    Serial.begin(115200);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Rtc.Begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    Rtc.Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        if (!wasError("setup IsDateTimeValid"))
        {
            Serial.println("RTC lost confidence in the DateTime!");
            Rtc.SetDateTime(compiled);
        }
    }

    if (!Rtc.GetIsRunning())
    {
        if (!wasError("setup GetIsRunning"))
        {
            Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("setup GetDateTime"))
    {
        if (now < compiled)
        {
            Serial.println("RTC is older than compile time, updating DateTime");
            Rtc.SetDateTime(compiled);
        }
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
    wasError("setup SetSquareWavePin");

/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    {
        Settings mySettings =
        {
            sizeof(Settings), // size
            42, // value1
            420, // value2
            18875, // value3
            3.14159f, // value4
        };

        // store the settings, nothing longer than 32 bytes due to I2C buffer
        uint8_t written = Rtc.SetMemory(SettingsAddress,
            reinterpret_cast<const uint8_t*>(&mySettings),
            sizeof(mySettings));
        wasError("setup setMemory settings");
        if (written != sizeof(mySettings))
        {
            Serial.println("setup setMemory failed to write complete settings");
        }
    }
/* end of comment out section */
}

void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        if (!wasError("loop IsDateTimeValid"))
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("loop GetDateTime"))
    {
        printDateTime(now);
        Serial.println();
    }

    delay(5000);

    // read data
    {
        Settings retrievedSettings = { 0,0,0,0,0 }; // init size to zero

        // get our data from the address with the given size
        uint8_t gotten = Rtc.GetMemory(SettingsAddress,
            reinterpret_cast<uint8_t*>(&retrievedSettings),
            sizeof(Settings));
        if (!wasError("loop getMemory settings"))
        {
            if (gotten != sizeof(Settings) ||
                retrievedSettings.size != sizeof(Settings))
            {
                Serial.print("something didn't match, requested = ");
                Serial.print(sizeof(Settings));
                Serial.print(", gotten = ");
                Serial.print(gotten);
                Serial.print(", size = ");
                Serial.print(retrievedSettings.size);
                Serial.println();
            }
            Serial.print("data read (");
            Serial.print(gotten);
            Serial.println(")");
            Serial.print("    size = ");
            Serial.println(retrievedSettings.size);
            Serial.print("    value1 = ");
            Serial.println(retrievedSettings.value1);
            Serial.print("    value2 = ");
            Serial.println(retrievedSettings.value2);
            Serial.print("    value3 = ");
            Serial.println(retrievedSettings.value3);
            Serial.print("    value4 = ");
            Serial.println(retrievedSettings.value4);
            Serial.println();
        }
    }

    
    delay(5000);
}

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

