# Rtc

Arduino Real Time Clock library.  
An RTC library with deep device support.

[![Donate](http://img.shields.io/paypal/donate.png?color=yellow)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=6AA97KE54UJR4)

Now supports esp8266.

## Installing This Library

Clone (Git) this project into Documents/Arduino/libraries/Rtc.  
It should now show up in the import list.

## Supported Devices
###DS1307
Full support including squarewave output pin and memory access

###DS3231 
Full support including squarewave output pin and alarms.

## Samples
###DS1307 Simple
This demonstrates how to set and get the date and time from the device.

###DS1307 Memory
This demonstrates how to write and read data from the extended memory of the device.  This memory is retained even after the device has lost power.

###DS3231 Simple
This demonstrates how to set and get the date and time from the device.

###DS3231 StoreIt
This demonstrates how to turn off the Rtc clock so that the onboard battery will last longer.

###DS3231 Alarms
This demonstrates how to set the alarms and check when the alarms are triggered.  This includes using the squarewave output to trigger an interrupt.

## Connecting the Devices
The RTC devices expose two digital wires labeled SDA and SCL.  These need to be connected to the wires exposed by your Arduino board labled the same way.  This varies from board to board so you will need to consult the Arduino reference documents for which pins are the SDA and SCL.
For ESP8266, these default to SDA = GPIO04 and SCL = GPIO05.
The RTC devices also require power.  Make sure that VCC is connected to the proper voltage that your device requires.  DS1307 requires 5v while the DS3231 can use either 3.3v or 5v.  The GND must be connected to the Arduino GND even if you are not powering the RTC from the Arduino voltage pins.
