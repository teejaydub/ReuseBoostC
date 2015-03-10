// App-specific constants for the LCDMenu module.

#include "Mancala.h"

// Must be provided elsewhere or defined inline here.
// Waits for a button to be pressed, and returns its value.
// Values returned are below.
inline char kb_getc(void)
{
	return WaitForInput(NO_BTN);
}

#define LEFT_KEY  PREV_BTN
#define ENTER_KEY  SELECT_BTN
#define RIGHT_KEY  NEXT_BTN
