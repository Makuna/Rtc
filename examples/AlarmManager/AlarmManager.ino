#include "RtcAlarmManager.h"
#include <Wire.h> 
#include <RtcDS1307.h> // Replace with the RTC you have

// foreward declare our alarm manager callback
void alarmCallback(uint8_t index, const RtcDateTime& alarm);
// global instance of the manager with three possible alarms
RtcAlarmManager<alarmCallback> Alarms(3);

// Replace with the RTC you have
RtcDS1307<TwoWire> Rtc(Wire);

// our three alarms
int8_t idAlarmSync;
int8_t idAlarmDaily;
int8_t idAlarmWeekly;

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

    // add an alarm to sync time from rtc at a regular interval,
    // due to CPU timing variance, the Alarms time can get off over
    // time, so this alarm will trigger a resync every 20 minutes 
    idAlarmSync = Alarms.AddAlarm(now, 20 * c_MinuteAsSeconds); // every 20 minutes

    // add a daily alarm at 5:30am
    RtcDateTime working(now.Year(), now.Month(), now.Day(), 5, 30, 0);
    idAlarmDaily = Alarms.AddAlarm(working, AlarmPeriod_Daily);

    // add a weekly alarm for Saturday at 7:30am
    working = RtcDateTime(now.Year(), now.Month(), now.Day(), 7, 30, 0);
    working = working.NextDayOfWeek(DayOfWeek_Saturday);
    idAlarmWeekly = Alarms.AddAlarm(working, AlarmPeriod_Weekly);

    Serial.println("Running...");
}

void loop () 
{
    delay(1000); // simulating other work your sketch will do
    Alarms.ProcessAlarms();
}

void alarmCallback(uint8_t index, [[maybe_unused]] const RtcDateTime& alarm)
{
    if (index == idAlarmSync)
    {
        // periodic sync from trusted source to minimize
        // drift due to inaccurate CPU timing
        RtcDateTime now = Rtc.GetDateTime();
        Alarms.Sync(now);
    }
    else if (index == idAlarmDaily)
    {
        Serial.println("ALARM: Its 5:30am!");
    }
    else if (index == idAlarmWeekly)
    {
        Serial.println("ALARM: Its Saturday at 7:30am!");
    }
}
