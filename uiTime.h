/* uiTime.h
    Copyright (c) 2007-2008 by Timothy J. Weber, tw@timothyweber.org.

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

// Simple, human-level timekeeping for user interface purposes.
// Resolution is milliseconds and seconds under a minute; 
// accuracy is dependent on the driving method.
//
// Can be driven off of Timer 0 or Timer 1 with most of the overhead handled here,
// or from a 60 Hz external signal.

#ifndef __UITIME_H
#define __UITIME_H

#ifdef IN_UITIME
 #define UITIME_EXTERN
#else
 #define UITIME_EXTERN  extern
#endif


// Count of ticks since startup.
// A "tick" happens roughly four times per second.
// Rolls over about every 64 - 67 seconds.
// Seems to be a convenient time unit for many UI-related short delays.
//
// Always use the difference between the current value of ticks and some previously-stored value;
// that way, you never have to worry about when it rolls over.
//
// Only change this value if it's only used to mean one thing in your app.
// If you only need one UI timer, you can save one byte of RAM that way (FWTW).
//
// ASSUMES A CLOCK RATE OF 4 MHz at the moment!
UITIME_EXTERN unsigned char ticks;

// Resets the timer to 0.
void ResetUITimer(void);

// Express your desired timeouts and delay factors in terms of this.
#define TICKS_PER_SEC  4
#define LOG2_TICKS_PER_SEC  2


//====================================================================
// Routines for using Timer 0 or 1 as the time source

// Rolls over every "tick".
// A tick is 0.262 seconds; there are about 3.8 per second.
// Driven by Timer 0.
UITIME_EXTERN unsigned char tickScaler;

// Initializes, and dedicates Timer 0 for use and maintenance by this module.
// Requires that GIE is enabled elsewhere, and that UiTimeInterrupt is called.
void InitUiTime_Timer0(void);

// Same, but use if the clock speed is 8 MHz.
void InitUiTime_Timer0_8MHz(void);

// Initializes, and dedicates Timer 1 for use and maintenance by this module.
// Requires that GIE is enabled elsewhere, and that UiTimeInterrupt1 is called.
void InitUiTime_Timer1(void);

// Call this in your interrupt handler if using Timer 0.
// Returns true about once a millisecond, which can be used for other tasks.
// E.g.:
//
//	void interrupt(void)
//	{
//		if (UiTimeInterrupt())
//			CheckButtons();
//	}
inline unsigned char UiTimeInterrupt(void)
{
	// Timer 0, rolling over with a 1.024 ms period.
	if (intcon.T0IF) {
		// Clear the interrupt.
		intcon.T0IF = 0;

		if (++tickScaler == 0)
			ticks++;

		return true;
	} else
		return false;
}

// Call this in your interrupt handler if using Timer 1.
void UiTimeInterrupt1(void);


//====================================================================
// Routines for using an external 60 Hz time source

// Call this once to initialize.
void InitUiTime_60Hz(void);

// Call this at 60 Hz to update.
void UiTimeUpdate60(void);


//====================================================================
// Routines for using an external time source that updates at any given frequency.

// Call this once to initialize.
// For NBy4, pass the update frequency in Hz divided by 4.
// You'll then get 4 ticks per second.
// In other words, pass the frequency of updates in updates per tick.
void InitUiTime_Freq(unsigned char NBy4);

// Call this at the specified frequency to update.
void UiTimeUpdateFreq(void);


//====================================================================
// Routines for using a 256-cycle timer like Timer 0 as a time source,
// but managed elsewhere.

// Call this once to initialize.
void InitUiTime256(void);

// Call this every 256 cycles to update.
// Returns true about once a millisecond, which can be used for other tasks.
unsigned char UiTimeUpdate256(void);


#endif