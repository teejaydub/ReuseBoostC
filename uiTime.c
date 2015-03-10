/* uiTime.c
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

#define IN_UITIME

#include <system.h>

#include "uiTime.h"


// Compatibility defines for 18F series.
#if defined(_PIC12F675) || defined(_PIC16F916) || defined(_PIC16F688) || defined(_PIC12F683) || defined(_PIC16F883) || defined(_PIC16F886) || defined(_PIC18F2550) || defined(_PIC18F2620)
	// These use the default T0IF.
#elif defined(_PIC18F2320) || defined(_PIC18F1320)
	#define T0IF  TMR0IF
#else
	#error "uiTime.c - update for this chip"
#endif


void ResetUITimer(void)
{
	tickScaler = 0;
	ticks = 0;
}

void InitUiTime_Timer0(void)
{
	#if defined(_PIC12F675) || defined(_PIC16F916) || defined(_PIC16F688) || defined(_PIC12F683) || defined(_PIC16F883) || defined(_PIC16F886)
	option_reg.T0CS = 0;  // T0 transition on internal CLKOUT
	option_reg = (option_reg & 0xF0) | 0x01;  // 1:4 prescaler on Timer 0: rolls over with a period of 1.024 ms.
	intcon.T0IE = 1;
	#elif defined(_PIC18F2620) || defined(_PIC18F2320) || defined(_PIC18F1320) || defined(_PIC18F2550)
	// Enable the timer 0 interrupt, and set prescaler.
	t0con = 0xC1;  // 1:4 prescaler on an 8-bit Timer 0: rolls over with a period of 1.024 ms.
	intcon.TMR0IE = 1;
	#else
		#error "uiTime.c - update for this chip"
	#endif

	intcon.PEIE = 1;
	tickScaler = 0;
	ticks = 0;
}

void InitUiTime_Timer0_8MHz(void)
{
	// Enable the timer 0 interrupt, and set prescaler.
	#if defined(_PIC18F2620) || defined(_PIC18F2320) || defined(_PIC18F1320) || defined(_PIC18F2550)
	t0con = 0xC2;  // 1:8 prescaler on an 8-bit Timer 0: rolls over with a period of 1.024 ms.
	intcon.TMR0IE = 1;
	#elif defined(_PIC16F688)
	option_reg = (option_reg & 0b11000000) | 0b000000  // Timer 0 counts instructions
		| 0b0000  // Timer 0 ges the 8-bit prescaler, not the watchdog
		| 0b010;  // Prescaler 1:8
	#else
		#error "uiTime.c - update for this chip"
	#endif

	intcon.PEIE = 1;
	tickScaler = 0;
	ticks = 0;
}

void InitUiTime_Timer1(void)
{
	pie1.TMR1IE = 1;
	intcon.PEIE = 1;
	t1con = 0x01;  // prescale 1:1 on 16-bit counter @ 1 MHz.
		// 1:4 would get you ~4 Hz, but we count those manually for more resolution.
	tickScaler = 0;
	ticks = 0;
}

void UiTimeInterrupt1(void)
{
	// Timer 1, rolling over with a 65.536 ms period.
	if (pir1.TMR1IF) {
		// Clear the interrupt.
		pir1.TMR1IF = 0;

		++tickScaler;
		if (tickScaler >= 4) {
			ticks++;
			tickScaler = 0;
		}
	} 
}

void InitUiTime_60Hz(void)
{
	ResetUITimer();
}

void UiTimeUpdate60(void)
{
	if (++tickScaler >= 15) {
		tickScaler = 0;
		ticks++;
	}
}

unsigned char  tickScaleVal;
void InitUiTime_Freq(unsigned char  NBy4)
{
	tickScaleVal = NBy4;
	ResetUITimer();
}

void UiTimeUpdateFreq(void)
{
	if (++tickScaler >= tickScaleVal) {
		tickScaler = 0;
		ticks++;
	}
}

void InitUiTime256(void)
{
	ResetUITimer();
}

unsigned char UiTimeUpdate256(void)
{
	static unsigned char postScaler4 = 0;
	
	if (++postScaler4 >= 4) {
		postScaler4 = 0;

		if (++tickScaler == 0)
			ticks++;
		return true;
	} else
		return false;
}
