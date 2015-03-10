/* atod.h
    Copyright (c) 2009-2012 by Timothy J. Weber, tw@timothyweber.org.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Encapsulates selecting and reading an A-to-D channel.
// Requires types-tjw.


#ifndef __ATOD_H
#define __ATOD_H

#include "fixed16.h"
#include "types-tjw.h"


// Initializes the conversion clock to the correct value for the given device frequency, passed as a constant.
#if defined(_PIC16F886) || defined(_PIC16F688) || defined(_PIC16F916)
 #define ADC_CLOCK_1MHZ  0b00000000
 #define ADC_CLOCK_4MHZ  0b01000000
 #define ADC_CLOCK_8MHZ  0b10000000
 #define ADC_CLOCK_RC  0b11000000
 inline void InitADClock(byte adcClock)
 {
	 adcon0 = (adcon0 & 0b00111111) | adcClock;
 }
#elif defined(_PIC12F683)
 #define ADC_CLOCK_1MHZ  0b01000000
 #define ADC_CLOCK_4MHZ  0b01010000
 #define ADC_CLOCK_8MHZ  0b00100000
 #define ADC_CLOCK_RC  0b01110000
 inline void InitADClock(byte adcClock)
 {
	 ansel = (ansel & 0b00001111) | adcClock;
 }
#else
 #error "Need to define how many channels this PIC has."
#endif

// Select the given A/D channel.
// Also requires that the corresponding TRIS register bit be set.
void SetADChannel(byte channel);

// Sets up for 1 = right-justified (maximum range, low bits dropped),
// or 0 = left-justified (low bits are significant, smaller output range).
// Relevant only when reading a 16-bit result; use left-justified (the default) for 8-bit results.
#define ADC_LEFT_JUSTIFIED  0
#define ADC_RIGHT_JUSTIFIED  1
inline void SetADJustification(byte isRightJustified)
{
#if defined(_PIC16F886)
	adcon1.ADFM = isRightJustified;
#elif defined(_PIC16F688) || defined(_PIC16F916) || defined(_PIC12F683)
	adcon0.ADFM = isRightJustified;
#else
 #error "Need to define how to set the justification on this PIC."
#endif
}

// Sets the specified A/D channel back to "digital" mode.
void TurnOffADChannel(byte channel);

// Waits the required settling time, starts a conversion, and waits for it to finish.
// Assumes the A/D channel has already been set up and selected.
// The result will be in adresh/adresl.
void AcquireAndConvertAD(void);

// Reads the given A/D channel.
inline void ReadADChannel(byte channel)
{
	SetADChannel(channel);
	AcquireAndConvertAD();
}

// Sets and acquires the given A/D channel and returns its most-significant 8 bits.
// Values will range from 0-255.
inline byte GetADValue8(byte channel)
{
	SetADJustification(ADC_LEFT_JUSTIFIED);
	ReadADChannel(channel);
	return adresh;
}

// Same, for unsigned short.
// Values will range from 0 - 1023, for a 10-bit A/D with right-justification,
// or 0 - 65535, for left-justification (with the bottom 6 bits = 0).
#define ADC_MAX_SHORT_LEFT  65535
#define ADC_MAX_SHORT_RIGHT  1023
#define ADC_RANGE_SHORT_LEFT  65536
#define ADC_RANGE_SHORT_RIGHT  1024
inline unsigned short GetADValueShort(byte channel)
{
	ReadADChannel(channel);
	
	unsigned short result;
	MAKESHORT(result, adresl, adresh);
	return result;
}

// Same, for fixed16.
// Values will range from 0.0 - 255.0 with left-justification (and two bits of fractional value),
// or 0.0 - 3.996.
inline fixed16 GetADValueFixed(byte channel)
{
	ReadADChannel(channel);
	
	fixed16 result;
	MAKE_FIXED(result, adresh, adresl);
	return result;
}

#endif