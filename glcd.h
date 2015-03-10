/* Graphic LCD code.
   For SparkFun's LCD-00710, a 128 x 64 module with an 8-bit parellel interface.
   Based on http://jormungand.net/projects/devices/avrlcd/
*/

#ifndef __GLCD_H
#define __GLCD_H  1

#include "types-tjw.h"


#define LCD_WIDTH  (64 * 2)
#define LCD_HEIGHT  64

// Define this to have all pixel- and text-oriented operations work upside down.
#define UPSIDE_DOWN

// Size of the standard character cell.
#define LCD_CHAR_WIDTH  6
#define LCD_CHAR_HEIGHT  8

#define LCD_COLUMNS  (LCD_WIDTH / LCD_CHAR_WIDTH)
#define LCD_ROWS  (LCD_HEIGHT / LCD_CHAR_HEIGHT)

/* glcd_init(): initialize lcd port directions
 *             enable both display chips
 *             set display line to zero */
void glcd_init(void);

/* glcd_clear(): clear the entire lcd to the given value */
void glcd_clear(byte data);

/* glcd_setbit(): sets the bit at (x,y) to value v
 *               may not appear until lcd_flush() is called. */
void glcd_setbit(byte x, byte y, byte v);

/* glcd_flush(): write pending bits out to display */
void glcd_flush(void);

#define CHARACTER_STUFF
#ifdef CHARACTER_STUFF

/* glcd_set_cursor(): set the cursor position to the given 6x8 cell */
void glcd_set_cursor(byte row, byte col);

/* glcd_putch(): write an ascii character at the cursor and advance */
void glcd_putch(byte ch);

/* glcd_puts(): write a string at the cursor, and advance
	Supports \n for newline.
 */ 
void glcd_puts(const char* str);

// Clears the entire 0-relative text line to 0 (white).
void glcd_clear_line(byte line);

#endif
#endif