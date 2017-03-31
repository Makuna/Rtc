
#ifndef __RTCUTILITY_H__
#define __RTCUTILITY_H__

// for some reason, the DUE board support does not define this, even though other non AVR archs do
#ifndef _BV
#define _BV(b) (1UL << (b))
#endif

extern uint8_t BcdToUint8(uint8_t val);
extern uint8_t Uint8ToBcd(uint8_t val);
extern uint8_t BcdToBin24Hour(uint8_t bcdHour);

#endif // __RTCUTILITY_H__
