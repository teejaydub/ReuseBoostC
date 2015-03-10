/* BlockingSound.c
    Copyright (c) 2007 by Timothy J. Weber, tw@timothyweber.org.

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

#include <system.h>

#include "BlockingSound.h"
#include "BlockingSoundConsts.h"
#include "types-tjw.h"

void PlaySound(unsigned short periodUs, unsigned short durationMs)
{
	// Prepare the port.
	SOUND_TRIS.SOUND_PIN = 0;
	
	// Think in terms of half-period, not periods.
	periodUs >>= 1;
	
	// Cut the period into two byte values we can pass to the standard delay functions.
	byte periodm = periodUs / 1000;
	byte period10u = (periodUs % 1000) / 10;
	
	intcon.GIE = 0;
	
	// Loop playing single cycles until we've done the required duration.
	// But, round to the nearest zero-going half-cycle, so we can leave the pin low.
	// (May save on power in certain circumstances?)
	unsigned short usPlayed = 0;
	unsigned short msPlayed = 0;
	while (msPlayed < durationMs || SOUND_LATCH.SOUND_PIN) {
		// Play for one period.
		
		// Toggle the sound pin.
		SOUND_LATCH ^= SOUND_MASK;
		
		// Delay.
		if (periodm)
			delay_ms(periodm);
		if (period10u)
			delay_10us(period10u);
		
		// Accumulate the total time.
		usPlayed += periodUs;
		
		while (usPlayed > 1000) {
			usPlayed -= 1000;
			++msPlayed;
		}
		
		// Feed the watchdog.
		clear_wdt();
	}		
	
	intcon.GIE = 1;
}

void PlayClick(void)
{
	// Prepare the port.
	SOUND_TRIS.SOUND_PIN = 0;
	
	// Just toggle the pin state once.
	//SOUND_LATCH ^= SOUND_MASK;
	
	PlaySound(1000, 2);
}

#define NUM_NOTES  24
unsigned short notePeriods[NUM_NOTES] = {
	3823,
	3608,
	3405,
	3214,
	3034,
	2864,
	2703,
	2551,
	2408,
	2273,
	2241,
	2025,
	1911,
	1804,
	1703,
	1607,
	1517,
	1432,
	1351,
	1276,
	1204,
	1136,
	1073,
	1066
};

// Given one of the note length codes,
// return the total time alloted to that note length,
// at a standard tempo (150 bpm), in milliseconds.
inline unsigned short TimeFor(char noteLengthCode)
{
	switch (noteLengthCode) {
	case '1':
		return 1600;
	case '2':
		return 800;
	case '4':
		return 400;
	case '8':
		return 200;
	case '6':
		return 100;
	case '3':
		return 50;
	default:
		return 0;
	}
}

void PlaySong(const char* song)
{
	unsigned short noteTimeMs = TimeFor('8');
	unsigned short noteDurationMs = noteTimeMs * 4 / 5;
	
	// This is a modifier added to the next note.
	char note = 0;
	byte playNote = false;
	
	for (; *song; ++song) {
		switch (*song) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '8':
		case '6':
			noteTimeMs = TimeFor(*song);
			noteDurationMs = noteTimeMs * 4 / 5;
			break;
			
		case '+':
			++note;
			break;
		
		case '-':
			--note;
			break;
			
		case 'c':
			playNote = true;
			break;
		case 'd':
			note += 2;
			playNote = true;
			break;
		case 'e':
			note += 4;
			playNote = true;
			break;
		case 'f':
			note += 5;
			playNote = true;
			break;
		case 'g':
			note += 7;
			playNote = true;
			break;
		case 'a':
			note += 9;
			playNote = true;
			break;
		case 'b':
			note += 11;
			playNote = true;
			break;
		case 'C':
			note += 12;
			playNote = true;
			break;
		case 'D':
			note += 14;
			playNote = true;
			break;
		case 'E':
			note += 16;
			playNote = true;
			break;
		case 'F':
			note += 17;
			playNote = true;
			break;
		case 'G':
			note += 19;
			playNote = true;
			break;
		case 'A':
			note += 21;
			playNote = true;
			break;
		case 'B':
			note += 23;
			playNote = true;
			break;
		}
	
		if (playNote) {
			if (note < NUM_NOTES) {
				unsigned short notePeriod = notePeriods[note];
			
				PlaySound(notePeriod, noteDurationMs);
				delay_ms(noteTimeMs - noteDurationMs);
			}
			
			note = 0;
			playNote = false;
		}
	}
}
