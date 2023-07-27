
// CONNECTIONS:
// At24c32 SDA --> SDA
// At24c32 SCL --> SCL
// At24c32 VCC --> 5v
// At24c32 GND --> GND

/* for software wire use below
#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <EepromAT24C32.h>

SoftwareWire myWire(SDA, SCL);
EepromAt24c32<SoftwareWire> Rtc(myWire);
/* for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <EepromAT24C32.h>

EepromAt24c32<TwoWire> RtcEeprom(Wire);

// if the above doesn't work, and you don't have "address" pads or switches on 
// the board to set the address, then try the common address of 7
// EepromAt24c32<TwoWire> RtcEeprom(Wire, 0b111);
// 
// If you have any of the address pads on the board soldered together
// then you need to provide the state of those pads, normally they
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

// example settings objec that will be serialized into and out of EEPROM
struct Settings
{
    uint8_t size; // size of this structure for versioning/validation
    uint8_t value1 : 6; // only 6 bits (0-63)
    uint8_t value2 : 6; // only 6 bits (0-63)
    uint8_t value3 : 6; // only 6 bits (0-63)
    uint32_t value4;
};

// where in EEPROM we will store the settings
const uint8_t SettingsAddress = 0;

// handy routine to return true if there was an error
// but it will also print out an error message with the given topic
bool wasError(const char* errorTopic = "")
{
    uint8_t error = RtcEeprom.LastError();
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

    //--------RTC EEPROM SETUP ------------
    // if you are using ESP-01 then uncomment the line below to define pins for
    // those available for SDA, SCL
    // RtcEeprom.Begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    RtcEeprom.Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the Eeprom
    {
        Settings mySettings =
            {
                sizeof(Settings), // size
                42, // value1
                21, // value2
                7, // value3
                18875, // value4
            };

        // store the settings, nothing longer than 32 bytes due to paging
        uint8_t written = RtcEeprom.SetMemory(SettingsAddress,
                reinterpret_cast<const uint8_t*>(&mySettings), 
                sizeof(mySettings));
        wasError("setup setMemory settings");
    }
/* end of comment out section */
}

void loop () 
{
    delay(5000);

    // read data
    {
        Settings retrievedSettings = { 0, 0, 0, 0, 0 }; // init to zero

        // get our data from the address with the given size
        uint8_t gotten = RtcEeprom.GetMemory(SettingsAddress, 
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
}


