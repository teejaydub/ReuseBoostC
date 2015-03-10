// lcd_consts.h
// Display configuration required for the lcd_driver header.
// Include this before lcd_driver to get a consistent set of parameters across the app.

#define DISPLAY_WIDTH  16

#define LCD_ARGS  0,	/* Interface type: mode 0 = 8bit, 1 = 4bit(low nibble), 2 = 4bit(upper nibble) */ \
		1, 				/* Use busy signal: 1 = use busy, 0 = use time delays */\
		PORTC, TRISC, 	/* Data port and data port tris register */ \
		PORTA, TRISA, 	/* Control port and control port tris register */ \
		2,				/* Bit number of control port is connected to RS */ \
		1,				/* Bit number of control port is connected to RW */ \
		0 				/* Bit number of control port is connected to Enable */
