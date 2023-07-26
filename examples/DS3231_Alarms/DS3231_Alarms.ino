
// CONNECTIONS:
// DS3231 SDA --> SDA
// DS3231 SCL --> SCL
// DS3231 VCC --> 3.3v or 5v
// DS3231 GND --> GND
// SQW --->  (Pin19) Don't forget to pullup (4.7k to 10k to VCC)

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


// Interrupt Pin Lookup Table
// (copied from Arduino Docs)
//
// CAUTION:  The interrupts are Arduino numbers NOT Atmel numbers
//   and may not match (example, Mega2560 int.4 is actually Atmel Int2)
//   this is only an issue if you plan to use the lower level interupt features
//
// Board           int.0    int.1   int.2   int.3   int.4   int.5
// ---------------------------------------------------------------
// Uno, Ethernet    2       3
// Mega2560         2       3       21      20     [19]      18 
// Leonardo         3       2       0       1       7

#define RtcSquareWavePin 19 // Mega2560
#define RtcSquareWaveInterrupt 4 // Mega2560

// marked volatile so interrupt can safely modify them and
// other code can safely read and modify them
volatile uint16_t interuptCount = 0;
volatile bool interuptFlag = false;

void ISR_ATTR InteruptServiceRoutine()
{
    // since this interupted any other running code,
    // don't do anything that takes long and especially avoid
    // any communications calls within this routine
    interuptCount++;
    interuptFlag = true;
}

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

    // set the interupt pin to input mode
    pinMode(RtcSquareWavePin, INPUT);

    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    Rtc.Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

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
    
    Rtc.Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmBoth); 
    wasError("setup SetSquareWavePin");

    // Alarm 1 set to trigger every day when 
    // the hours, minutes, and seconds match
    RtcDateTime alarmTime = now + 88; // into the future
    DS3231AlarmOne alarm1(
            alarmTime.Day(),
            alarmTime.Hour(),
            alarmTime.Minute(), 
            alarmTime.Second(),
            DS3231AlarmOneControl_HoursMinutesSecondsMatch);
    Rtc.SetAlarmOne(alarm1);
    wasError("setup SetAlarmOne");

    // Alarm 2 set to trigger at the top of the minute
    DS3231AlarmTwo alarm2(
            0,
            0,
            0, 
            DS3231AlarmTwoControl_OncePerMinute);
    Rtc.SetAlarmTwo(alarm2);
    wasError("setup SetAlarmTwo");

    // throw away any old alarm state before we ran
    Rtc.LatchAlarmsTriggeredFlags();
    wasError("setup LatchAlarmsTriggeredFlags");

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
    // setup external interupt 
    // for some Arduino hardware they use interrupt number for the first param
    attachInterrupt(RtcSquareWaveInterrupt, InteruptServiceRoutine, FALLING);
#else
    // for some Arduino hardware they use interrupt pin for the first param
    attachInterrupt(RtcSquareWavePin, InteruptServiceRoutine, FALLING);
#endif
}

void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        if (!wasError("loop IsDateTimeValid"))
        {
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("loop GetDateTime"))
    {
        printDateTime(now);
        Serial.println();
    }

    // we only want to show time every 10 seconds
    // but we want to show responce to the interupt firing
    for (int timeCount = 0; timeCount < 20; timeCount++)
    {
        if (Alarmed())
        {
            Serial.print(">>Interupt Count: ");
            Serial.print(interuptCount);
            Serial.println("<<");
        }
        delay(500);
    }
}

bool Alarmed()
{
    bool wasAlarmed = false;
    if (interuptFlag)  // check our flag that gets sets in the interupt
    {
        wasAlarmed = true;
        interuptFlag = false; // reset the flag
        
        // this gives us which alarms triggered and
        // then allows for others to trigger again
        DS3231AlarmFlag flag = Rtc.LatchAlarmsTriggeredFlags();
        if (!wasError("alarmed LatchAlarmsTriggeredFlags"))
        {
            if (flag & DS3231AlarmFlag_Alarm1)
            {
                Serial.println("alarm one triggered");
            }
            if (flag & DS3231AlarmFlag_Alarm2)
            {
                Serial.println("alarm two triggered");
            }
        }
    }
    return wasAlarmed;
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

