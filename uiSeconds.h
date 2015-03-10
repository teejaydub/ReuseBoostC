/* uiSeconds.h
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

// Simple timekeeping; accumulates uiTime's clock ticks into seconds.
// Range is from one second to 2^16 seconds (~18 hours); accuracy is dependent on uiTime.
// Requires uiTime.

#ifndef __UISECONDS_H
#define __UISECONDS_H

#ifdef IN_UISECONDS
 #define UISECONDS_EXTERN
#else
 #define UISECONDS_EXTERN  extern
#endif

#include "uiTime.h"


// Count of tick-based seconds since startup, or since the counter was last cleared.
// 
// Rolls over about every 18 hours.
//
// Either use the difference between this and a previously-stored value to avoid 
// rollover problems, or clear it and use it directly (but be sure it's only used
// to mean one thing in your app).
UISECONDS_EXTERN unsigned short seconds;


// Call this to initialize the module or to reset the counter.
void ClearUiSeconds(void);

// Call this periodically to update the count.
// Just needs to be called with as much resolution as is needed for timekeeping.
// And, must be called at least once a minute.
// Returns true if we've just rolled over to a new second.
byte UpdateUiSeconds(void);


#endif