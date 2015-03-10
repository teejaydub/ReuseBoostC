/* dayTime.h
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
/*	
	Stores and operates on the time of day.
	Stored in an unsigned short as minutes since midnight.
	
	One master time value is presented for use as the current time.
	You can also convert values to time types, and do math on them.
	
	Gets time from uiSeconds and uiTime.  Takes over uiSeconds's
	global 'seconds' for use as the current seconds, 0-59.
*/

#ifndef __DAYTIME_H
#define __DAYTIME_H


#include "types-tjw.h"


#define SECONDS_PER_MINUTE  60
#define MINUTES_PER_HOUR  60
#define HOURS_PER_DAY  24
#define MINUTES_PER_DAY  (MINUTES_PER_HOUR * HOURS_PER_DAY)


#ifdef IN_DAYTIME
 #define DAYTIME_EXTERN
#else
 #define DAYTIME_EXTERN  extern
#endif


typedef unsigned short  dayTime_t;


// This value is guaranteed never to be used for a valid dayTime.
#define INVALID_DAYTIME  0x05DC  // AKA 25 hours, 0 minutes.


// Don't change this outside of this module.
// Format is minutes since midnight.
DAYTIME_EXTERN dayTime_t currentTime;


inline dayTime_t MakeDayTime(byte hours, byte minutes)
{
	return (unsigned short)(hours) * MINUTES_PER_HOUR + (unsigned short) minutes;
}

// Returns the given time, divided up into hours and minutes and returned in the two specified variables.
void DecodeDayTime(dayTime_t time, byte& hours, byte& minutes);

// Sets the current time, in a 24-hour clock.
void SetDayTime(byte hours, byte minutes);

// This must be called frequently to update the time - at least once per minute.
// It returns true if we've just crossed to a new day.
byte UpdateDayTime(void);

// Gets the current time, in a 24-hour clock, into the specified variables.
#define GetDayTime(hours, minutes)  DecodeDayTime(currentTime, hours, minutes)


#endif