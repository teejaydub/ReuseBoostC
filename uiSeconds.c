/* uiSeconds.c
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

#define IN_UISECONDS

#include <system.h>

#include "types-tjw.h"

#include "uiSeconds.h"



// Tick value when we last updated.
static byte secondTicks = 0;


void ClearUiSeconds(void)
{
	seconds = 0;
	secondTicks = ticks;
}

byte UpdateUiSeconds(void)
{
	byte result = false;
	
	byte elapsedTicks = ticks - secondTicks;
	while (elapsedTicks >= TICKS_PER_SEC) {
		++seconds;

		elapsedTicks -= TICKS_PER_SEC;
		secondTicks += TICKS_PER_SEC;
		
		result = true;
	}
}
