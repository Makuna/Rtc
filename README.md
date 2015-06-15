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
