/*-------------------------------------------------------------------------
RTC library

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/Rtc)

-------------------------------------------------------------------------
This file is part of the Makuna/Rtc library.

Rtc is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Rtc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Rtc.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#pragma once

// ESP32 complains if not included
#if defined(ARDUINO_ARCH_ESP32)
#include <inttypes.h>
#endif

#if !defined(countof)
#define countof(a) (sizeof(a) / sizeof(a[0]))
#endif

// Arduino has no standard for attributing methods used for ISRs
// even though some platforms require it, so to simplify the problem
// for end users, this provides a standard ISR_ATTR
#if !defined(ISR_ATTR)

#if defined(ARDUINO_ARCH_ESP8266) 
#define ISR_ATTR ICACHE_RAM_ATTR
#elif defined(ARDUINO_ARCH_ESP32)
#define ISR_ATTR ICACHE_RAM_ATTR
#else
#define ISR_ATTR
#endif

#endif // !defined(ISR_ATTR)

// some platforms do not come with STL or properly defined one, specifically functional
// if you see...
// undefined reference to `std::__throw_bad_function_call()'
// ...then you can either add the platform symbol to the list so RTC_NO_STL gets defined or
// go to boards.txt and enable c++ by adding (teensy31.build.flags.libs=-lstdc++) and set to "smallest code" option in Arduino
//
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(STM32L432xx) || defined(STM32L476xx) || defined(ARDUINO_ARCH_SAM)
#define RTC_NO_STL 1
#endif

// While WIRE has return codes, there is no standard definition of what they are
// within any headers; they are only documented on the website here
// https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
// So we define our own "standard" for this RTC library that match those
//
enum Rtc_Wire_Error
{
	Rtc_Wire_Error_None = 0,
	Rtc_Wire_Error_TxBufferOverflow,
	Rtc_Wire_Error_NoAddressableDevice,
	Rtc_Wire_Error_UnsupportedRequest,
	Rtc_Wire_Error_Unspecific,
	Rtc_Wire_Error_CommunicationTimeout
};

// for some reason, the DUE board support does not define this, even though other non AVR archs do
#ifndef _BV
#define _BV(b) (1UL << (b))
#endif

extern uint8_t BcdToUint8(uint8_t val);
extern uint8_t Uint8ToBcd(uint8_t val);
extern uint8_t BcdToBin24Hour(uint8_t bcdHour);



