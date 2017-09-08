
#ifndef __RTCUTILITY_H__
#define __RTCUTILITY_H__

// ESP32 complains if not included
#if defined(ARDUINO_ARCH_ESP32)
#include <inttypes.h>
#endif

// for some reason, the DUE board support does not define this, even though other non AVR archs do
#ifndef _BV
#define _BV(b) (1UL << (b))
#endif

extern uint8_t BcdToUint8(uint8_t val);
extern uint8_t Uint8ToBcd(uint8_t val);
extern uint8_t BcdToBin24Hour(uint8_t bcdHour);

#endif // __RTCUTILITY_H__
