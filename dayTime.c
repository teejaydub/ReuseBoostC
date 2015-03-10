/* dayTime.c
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

#define IN_DAYTIME

#include <system.h>

#include "dayTime.h"
#include "uiSeconds.h"


void SetDayTime(byte hours, byte minutes)
{
	seconds = 0;
	currentTime = MakeDayTime(hours, minutes);
}

byte UpdateDayTime(void)
{
	byte result = false;
	
	while (seconds >= SECONDS_PER_MINUTE) {
		intcon.GIE = 0;
		seconds -= SECONDS_PER_MINUTE;
		intcon.GIE = 1;
		
		// Add a minute.
		++currentTime;
		
		if (currentTime >= MINUTES_PER_DAY) {
			// We'll assume less than a minute has passed since we noticed.
			currentTime = 0;
			result = true;
		}
	}
	
	return result;
}

void DecodeDayTime(dayTime_t time, byte& hours, byte& minutes)
{
	hours = time / MINUTES_PER_HOUR;
	minutes = time % MINUTES_PER_HOUR;
}
