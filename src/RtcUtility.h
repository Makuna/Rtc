
#ifndef __RTCUTILITY_H__
#define __RTCUTILITY_H__

// ESP32 complains if not included
#if defined(ARDUINO_ARCH_ESP32)
#include <inttypes.h>
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


// for some reason, the DUE board support does not define this, even though other non AVR archs do
#ifndef _BV
#define _BV(b) (1UL << (b))
#endif

extern uint8_t BcdToUint8(uint8_t val);
extern uint8_t Uint8ToBcd(uint8_t val);
extern uint8_t BcdToBin24Hour(uint8_t bcdHour);

#endif // __RTCUTILITY_H__
