
// CONNECTIONS:
// PCF8563 SDA --> SDA
// PCF8563 SCL --> SCL
// PCF8563 VCC --> 3.3v or 5v
// PCF8563 GND --> GND
// PCF8563 INT --->  (Pin19) Don't forget to pullup (4.7k to 10k to VCC)

/* for software wire use below
#include <SoftwareWire.h>  // must be included here so that Arduino library object file references work
#include <RtcPCF8563.h>

SoftwareWire myWire(SDA, SCL);
RtcPCF8563<SoftwareWire> Rtc(myWire);
 for software wire use above */

/* for normal hardware wire use below */
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcPCF8563.h>
RtcPCF8563<TwoWire> Rtc(Wire);
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

#define RtcInterruptPin 19 // Mega2560
#define RtcInterruptInterrupt 4 // Mega2560

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
    pinMode(RtcInterruptPin, INPUT);

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
    
    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.StopAlarm();
    wasError("setup StopAlarm");
    Rtc.StopTimer();
    wasError("setup StopTimer");
    Rtc.SetSquareWavePin(PCF8563SquareWavePinMode_None);
    wasError("setup SetSquareWavePin");

    // Alarm set to trigger every day when 
    // the hours and minutes match
    RtcDateTime alarmTime = now + 88; // into the future
    PCF8563Alarm alarm(
            alarmTime.Day(), // will be ignored in this example
            alarmTime.Hour(),
            alarmTime.Minute(), 
            alarmTime.DayOfWeek(), // will be ignored in this example
            PCF8563AlarmControl_MinuteMatch | PCF8563AlarmControl_HourMatch
        );
    Rtc.SetAlarm(alarm);
    wasError("setup SetAlarm");

    // Timer set to trigger in 100 seconds
    //
    Rtc.SetTimer(PCF8563TimerMode_Seconds, 100);

  #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
    // setup external interupt 
    // for some Arduino hardware they use interrupt number for the first param
    attachInterrupt(RtcInterruptInterrupt, InteruptServiceRoutine, FALLING);
#else
    // for some Arduino hardware they use interrupt pin for the first param
    attachInterrupt(RtcInterruptPin, InteruptServiceRoutine, FALLING);
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
    bool result = false;
    if (interuptFlag)  // check our flag that gets sets in the interupt
    {
        interuptFlag = false; // reset the flag
        
        // calling LatchAlarmTriggeredFlag() will return if
        // the alarm was what triggered the interrupt and also
        // resets the alarm flag inside the rtc which then allows 
        // for alarms to trigger again.
        // note that the same int pin is also used for
        // the timer trigger if also used
        bool wasAlarmed = Rtc.LatchAlarmTriggeredFlag();
        if (!wasError("alarmed LatchAlarmTriggeredFlag"))
        {
            if (wasAlarmed)
            {
                result = true;
                Serial.println("alarm triggered");
            }
        }

        // calling LatchTimerTriggeredFlag() will return if
        // the timer was what triggered the interrupt and also
        // resets the timer flag inside the rtc which then allows 
        // for timers to trigger again.
        // note that the same int pin is also used for
        // the alarm trigger if also used
        bool wasTimerExpired = Rtc.LatchTimerTriggeredFlag();
        if (!wasError("alarmed LatchTimerTriggeredFlag"))
        {
            if (wasTimerExpired)
            {
                result = true;
                Serial.println("timer triggered");
            }
        }
    }
    return result;
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

