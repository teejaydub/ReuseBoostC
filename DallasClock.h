// Driver for DS1307 real-time clock communication.

#ifndef _DALLAS_CLOCK
#define _DALLAS_CLOCK

#include "types-tjw.h"


#ifdef IN_DALLAS_CLOCK
 #define DALLAS_CLOCK_EXTERN
#else
 #define DALLAS_CLOCK_EXTERN  extern
#endif

// Call this once, before other functions.
// Assumes complete control over the I2C bus.
// Returns false if the clock appears to be set to the epoch.
byte InitDallasClock(void);

// Returns the current value of the seconds register, as a number from 0-59.
byte GetClockSeconds(void);

// Reads 8 bytes into buffer.
void GetClockMemory(byte* buffer);

// Reads from the clock into this structure.
// Returns false on error.
DALLAS_CLOCK_EXTERN struct {
	byte seconds;  // BCD, 00-59
	byte minutes;  // BCD, 00-59
	byte hours;  // BCD, 00-12
	byte dayOfWeek;  // 1-7
	byte dayOfMonth;  // BCD, 01-31
	byte month;  // BCD, 01-12
	byte year;  // BCD, 00-99
} currentTime;


// Use this to access the currentTime as an array of bytes, like currentTimeBuf[0].
#define currentTimeBuf  ((byte*) &currentTime)

// Reads and writes the entire currentTime structure all at once.
byte ReadClock(void);
void WriteClock(void);

// Writes an individual clock register.
void SetClockRegister(byte index, byte newValue);

#endif