/* LCDUI.c
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
#include <string.h>

#include "lcd.h"
#include "LCDUI.h"
#include "LCDUIConsts.h"

rom char* okChar = {0x1c,0x14,0x1c,0x0,0x5,0x6,0x5};
rom char* ellipsisChar = {0x0,0x0,0x0,0x0,0x0,0x0,0x15};


void InitLCDUI(void)
{
	lcd_setup();

	lcd_set_char(OK_CHAR, okChar);
	lcd_set_char(ELLIPSIS_CHAR, ellipsisChar);
}

void HighlightStatusChar(void)
{
	lcd_gotoxy(DISPLAY_WIDTH - 1, 0);
}

void ShowStatusChar(char c)
{
	HighlightStatusChar();
	lcd_datamode();
	lcd_write(c);
}

// Prompts for, and wait for, the user to press Select to continue.
void ConfirmMessage(void)
{
	// Show the prompt character.
	ShowStatusChar(OK_CHAR);
	
	lcd_gotoxy(DISPLAY_WIDTH - 1, 0);

	// Wait.
	char c;
	do 
		c = kb_getc();
	while (c != ENTER_KEY);
}

void PrintCentered(const char* msg, byte field)
{
	byte leftPad = (field - strlen(msg)) / 2;
	
	lcd_datamode();
	
	byte i;
	for (i = 0; i < leftPad; i++)
		lcd_write(' ');
		
	lprintf(msg);
	
	i += strlen(msg);
	
	while (i++ < field)
		lcd_write(' ');
}

void DialogBox(const char* msg1, const char* msg2)
{
	lcd_clear();
	
	PrintCentered(msg1, DISPLAY_WIDTH);
	
	lcd_gotoxy(0, 1);
	PrintCentered(msg2, DISPLAY_WIDTH);
	
	ConfirmMessage();
}

/* Displays menus like this:

	Given: "New game...|Statistics...|Change sides|Game...|Sound...|Auto-off..."
	Shows:
		_N_SGSA?
		New game...
	
	where _ shows the position of the cursor.
	
	As the directional buttons are pressed, a different initial is highlighted
	on the first row, and that item's full description is shown 
	on the second row.
*/
byte GetMenuChoice(rom char* menuItems)
{	
	// Analyze the input.
	byte i;
	byte titleLength = 0;  // Nonzero iff there's a title.
	byte numItems = 0;
	for (i = 0; menuItems[i]; i++) {
		switch (menuItems[i]) {
		case ':':
			// Note the length of the title.
			titleLength = i + 1;
			if (menuItems[i + 1] != '\0')
				numItems = 1;
			else
				numItems = 0;
			break;
			
		case '|':
			// Count the items.
			if (menuItems[i + 1])
				++numItems;
			break;
		}
	}
	
	// Abort if there were no items.
	if (numItems == 0)
		return 0;
	
	// Clear the screen.
	lcd_clear();
	
	// Show the title, if it's given.
	lcd_datamode();
	for (i = 0; i < titleLength; i++)
		lcd_write(menuItems[i]);
	lcd_write(' ');
	
	// Display the list of choice initials.
	byte choiceListX;
	if (titleLength)
		choiceListX = titleLength + 1;
	else
		choiceListX = 0;
	
	for (; menuItems[i]; i++)
		if (i == 0 || menuItems[i - 1] == '|' || menuItems[i - 1] == ':')
			lcd_write(menuItems[i]);
	
	
	// Start selecting.
	byte currentChoice = 0;
	while (1) {
		// Index to the selected item's text.
		byte itemIndex = 0;
		for (i = titleLength; itemIndex < currentChoice; i++)
			if (menuItems[i] == '|')
				++itemIndex;
			
		// Display the current item's text on the second line.
		lcd_gotoxy(0, 1);
		lcd_datamode();
		byte x;
		for (x = 0; menuItems[i] && menuItems[i] != '|'; i++, x++)
			lcd_write(menuItems[i]);
			
		// Fill to the end of the line.
		for (; x < DISPLAY_WIDTH; x++)
			lcd_write(' ');

		// Move the cursor to the current choice initial.		
		lcd_gotoxy(choiceListX + currentChoice, 0);
	
		// Get and process the next key.
		switch (kb_getc()) {
		case LEFT_KEY:
			if (currentChoice == 0)
				currentChoice = numItems - 1;
			else
				--currentChoice;
			break;
		
		case RIGHT_KEY:
			if (currentChoice == numItems - 1)
				currentChoice = 0;
			else
				++currentChoice;
			break;
		
		case ENTER_KEY:
			return currentChoice;
			break;
		}
	}
}
