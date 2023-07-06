#include "RtcAlarmManager.h"
#include <Wire.h> 
#include <RtcDS1307.h> // Replace with the RTC you have

// foreward declare our alarm manager callback
void alarmCallback(uint8_t id, const RtcDateTime& alarm);
// global instance of the manager with three possible alarms
RtcAlarmManager<alarmCallback> Alarms(3);

// Replace with the RTC you have
RtcDS1307<TwoWire> Rtc(Wire);

void setup () 
{
    Serial.begin(115200);

    Serial.println("Initializing...");

    //--------RTC SETUP ------------
    Rtc.Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

    // get the real date and time from a source like an already
    // configured RTC module
    RtcDateTime now = Rtc.GetDateTime();
    // Sync the Alarms to current time
    Alarms.Sync(now);

    // NOTE:  Due to this sketch not deleting alarms, the returned ids from
    // AddAlarm can be assumed to start at zero and increment from there.
    // Otherwise the ids would need to be captured and used in the callback
    //
    int8_t result;
    // add an alarm to sync time from rtc at a regular interval,
    // due to CPU timing variance, the Alarms time can get off over
    // time, so this alarm will trigger a resync every 20 minutes 
    result = Alarms.AddAlarm(now, 20 * c_MinuteAsSeconds); // every 20 minutes
    if (result < 0) 
    {
        // an error happened
        Serial.print("AddAlarm Sync failed : ");
        Serial.print(result);
    }

    // add a daily alarm at 5:30am
    RtcDateTime working(now.Year(), now.Month(), now.Day(), 5, 30, 0);
    result = Alarms.AddAlarm(working, AlarmPeriod_Daily);
    if (result < 0)
    {
        // an error happened
        Serial.print("AddAlarm Daily failed : ");
        Serial.print(result);
    }

    // add a weekly alarm for Saturday at 7:30am
    working = RtcDateTime(now.Year(), now.Month(), now.Day(), 7, 30, 0);
    working = working.NextDayOfWeek(DayOfWeek_Saturday);
    result = Alarms.AddAlarm(working, AlarmPeriod_Weekly);
    if (result < 0)
    {
        // an error happened
        Serial.print("AddAlarm Weekly failed : ");
        Serial.print(result);
    }

    Serial.println("Running...");
}

void loop () 
{
    delay(1000); // simulating other work your sketch will do
    Alarms.ProcessAlarms();
}

void alarmCallback(uint8_t id, [[maybe_unused]] const RtcDateTime& alarm)
{
    // NOTE:  Due to this sketch not deleting alarms, the returned ids from
    // AddAlarm can be assumed to start at zero and increment from there.
    // Otherwise the ids would need to be captured and used here 
    //
    switch (id)
    {
    case 0:
        {   
            // periodic sync from trusted source to minimize
            // drift due to inaccurate CPU timing
            RtcDateTime now = Rtc.GetDateTime();
            Alarms.Sync(now); 
        }
        break;

    case 1:
        Serial.println("DAILY ALARM: Its 5:30am!");
        break;

    case 2:
        Serial.println("WEEKLY ALARM: Its Saturday at 7:30am!");
        break;
    }
}
