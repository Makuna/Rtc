
// Reference for connecting SPI see https://www.arduino.cc/en/Reference/SPI
// CONNECTIONS:
// DS3234 MISO --> MISO
// DS3234 MOSI --> MOSI
// DS3234 CLK  --> CLK (SCK)
// DS3234 CS (SS) --> 5 (pin used to select the DS3234 on the SPI)
// DS3234 VCC --> 3.3v or 5v
// DS3234 GND --> GND
// SQW --->  (Pin19) Don't forget to pullup (4.7k to 10k to VCC)

const uint8_t DS3234_CS_PIN = 5;

#include <SPI.h>
#include <RtcDS3234.h>

RtcDS3234<SPIClass> Rtc(SPI, DS3234_CS_PIN);

// Interrupt Pin Lookup Table
// (copied from Arduino Docs)
//
// CAUTION:  The interrupts are Arduino numbers NOT Atmel numbers
//   and may not match (example, Mega2560 int.4 is actually Atmel Int2)
//   this is only an issue if you plan to use the lower level interrupt features
//
// Board           int.0    int.1   int.2   int.3   int.4   int.5
// ---------------------------------------------------------------
// Uno, Ethernet    2       3
// Mega2560         2       3       21      20     [19]      18 
// Leonardo         3       2       0       1       7
// esp8266          (pin and interrupt should be the same thing)
// esp32            (pin and interrupt should be the same thing)

#define RtcSquareWavePin 19 // Mega2560
#define RtcSquareWaveInterrupt 4 // Mega2560

// marked volatile so interrupt can safely modify them and
// other code can safely read and modify them
volatile uint16_t interruptCount = 0;
volatile bool interruptFlag = false;

void ISR_ATTR interruptServiceRoutine()
{
    // since this interrupted any other running code,
    // don't do anything that takes long and especially avoid
    // any communications calls within this routine
    interruptCount++;
    interruptFlag = true;
}

void setup () 
{
    Serial.begin(115200);
    while (!Serial);

    // set the interrupt pin to input mode
    pinMode(RtcSquareWavePin, INPUT_PULLUP); // external pullup maybe required still

    SPI.begin();
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

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
    
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3234SquareWavePin_ModeAlarmBoth); 

    // Alarm 1 set to trigger every day when 
    // the hours, minutes, and seconds match
    RtcDateTime alarmTime = now + 88; // into the future
    DS3234AlarmOne alarm1(
            alarmTime.Day(),
            alarmTime.Hour(),
            alarmTime.Minute(), 
            alarmTime.Second(),
            DS3234AlarmOneControl_HoursMinutesSecondsMatch);
    Rtc.SetAlarmOne(alarm1);

    // Alarm 2 set to trigger at the top of the minute
    DS3234AlarmTwo alarm2(
            0,
            0,
            0, 
            DS3234AlarmTwoControl_OncePerMinute);
    Rtc.SetAlarmTwo(alarm2);

    // throw away any old alarm state before we ran
    Rtc.LatchAlarmsTriggeredFlags();

    // setup external interrupt 
    attachInterrupt(RtcSquareWaveInterrupt, interruptServiceRoutine, FALLING);
}

void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    // we only want to show time every 10 seconds
    // but we want to show response to the interrupt firing
    for (int timeCount = 0; timeCount < 20; timeCount++)
    {
        if (Alarmed())
        {
            Serial.print(">>interrupt Count: ");
            Serial.print(interruptCount);
            Serial.println("<<");
        }
        delay(500);
    }
}

bool Alarmed()
{
    bool wasAlarmed = false;
    if (interruptFlag)  // check our flag that gets sets in the interrupt
    {
        wasAlarmed = true;
        interruptFlag = false; // reset the flag
        
        // this gives us which alarms triggered and
        // then allows for others to trigger again
        DS3234AlarmFlag flag = Rtc.LatchAlarmsTriggeredFlags();

        if (flag & DS3234AlarmFlag_Alarm1)
        {
            Serial.println("alarm one triggered");
        }
        if (flag & DS3234AlarmFlag_Alarm2)
        {
            Serial.println("alarm two triggered");
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

