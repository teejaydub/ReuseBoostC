/* atod.c
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

// Encapsulates setting up and reading an A-to-D channel.
// Requires types-tjw.

#include <system.h>
#include <stdlib.h>

#include "atod.h"


#if defined(_PIC16F886)
 #define MORE_THAN_8_CHANNELS
#elif defined(_PIC16F688) || defined(_PIC16F916) || defined(_PIC12F683)
#else
 #error "Need to define how many channels this PIC has."
#endif


// Select the given A/D channel.
// Also requires that the corresponding TRIS register bit be set.
void SetADChannel(byte channel)
{
#ifdef MORE_THAN_8_CHANNELS
	if (channel > 7) {
		anselh = BITMASK(channel - 8);
	} else {
#endif
#ifdef _PIC12F683
		ansel = (ansel & 0x01110000) | BITMASK(channel);  // preserve the clock select bits, which are stored here
#else
		ansel = BITMASK(channel);
#endif
#ifdef MORE_THAN_8_CHANNELS
	}
#endif
	
	pir1.ADIF = 0;
	
	adcon0 = (adcon0 & 0b11000000) | (channel << 2);  // keep current justification and reference (or clock for 16F886), !ADON, !GO.
#ifdef _PIC16F886
	adcon1 = (adcon1 & 0b10000000);  // Ref = Vss to Vdd, preserve justification.
#endif
}

void TurnOffADChannel(byte channel)
{
#ifdef MORE_THAN_8_CHANNELS
	if (channel > 7) {
		anselh &= ~(BITMASK(channel - 8));
	} else {
#endif
		ansel &= ~(BITMASK(channel));
#ifdef MORE_THAN_8_CHANNELS
	}
#endif
	
	adcon0 = (adcon0 & 0b11000000) | (channel << 2);  // and right-justified, !ADON, !GO, keep the existing clock.
}

// Blocks the required settling time, starts a conversion, and waits for it to finish.
// Assumes the A/D channel has already been set up and selected.
// The result will be in adresh/adresl.
void AcquireAndConvertAD(void)
{
	// Turn on the A/D.
	adcon0.ADON = 1;

	// Wait for settling and acquisition.
	delay_us(40);
	
	// Start conversion.
	adcon0.GO_DONE = 1;
	
	// Wait for conversion to finish.
	while (adcon0.GO_DONE)
		clear_wdt();
	
	// Turn off A/D.
	adcon0.ADON = 0;
}
