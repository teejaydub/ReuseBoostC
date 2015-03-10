/* longPress.c
    Copyright (c) 2014 by Timothy J. Weber, tw@timothyweber.org.

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

#define IN_LONG_PRESS
#include "longPress.h"

#include "math-tjw.h"


// Global for the last tick count when all buttons were up.
byte ticksAllButtonsUp;
byte ticksPerLongPress = TICKS_PER_SEC;

// Button bit masks.
byte lastDowns;
byte shortPresses;
byte longPresses;
byte waitingForRelease;  // after a long press

void InitLongPress(byte newTicksPerLongPress)
{
	ticksPerLongPress = newTicksPerLongPress;
	ticksAllButtonsUp = ticks;
	lastDowns = 0;
	shortPresses = 0;
	longPresses = 0;
	waitingForRelease = false;
}

void CheckButtonReleases(void)
{
	CheckButtons();
	
	if (IS_ANY_BTN_DOWN) {
		lastDowns |= buttonsPressed;
		
		// If we've been holding it down long enough, note that it's a long press.
		if (!waitingForRelease && ticks - ticksAllButtonsUp >= ticksPerLongPress) {
			longPresses |= lastDowns;
			waitingForRelease = true;
		}
	} else {
		// No buttons are down.  Was one just released?
		if (lastDowns) {
			// Yes.  Note the timing.
			if (ticks - ticksAllButtonsUp < ticksPerLongPress)
				shortPresses |= lastDowns;
				
			lastDowns = 0;
		}
		waitingForRelease = false;
		ticksAllButtonsUp = ticks;
	}
}

byte GetShortRelease(void)
{
	return clearLowestSetBit<byte>(shortPresses, LAST_BTN);
}

byte GetLongPress(void)
{
	return clearLowestSetBit<byte>(longPresses, LAST_BTN);
}