////////////////////////////////////////////////////////////////////////////
// LCD with HD44780 drive chip
////////////////////////////////////////////////////////////////////////////
// Author(s): David Hobday, Pavel Baranov
// Date 15 November 2004
//
// Copyright (C) 2004-2006 Pavel Baranov
// Copyright (C) 2004-2006 David Hobday
// Copyright (C) 2004 Andrew Smallridge
// Modified 2007 by Timothy Weber - redistributed without permission, for use only with a valid BoostC license.
//
//
// How to use - by David Hobday
// ============================
// These must be define before template header file is included
// Remember when using LCD in 4 bit mode you must connect to the LCDs DB4-DB7 pins
//
// *** For a list of functions, see the END of this file.
//
// The time delays used in the code should mean that it will work on PIC18 with 
// 40MHz clock without any changes.
//
// The example below (which would work on a PIC16F84) operates the display 
// in 4bit mode and requires the following connections:
// RS  to RA3
// R/W to RA2
// E   to RA1
// DB0 to None
// DB1 to None
// DB2 to None
// DB3 to None
// DB4 to RB4
// DB5 to RB5
// DB6 to RB6
// DB7 to RB7
//
//
// Add the following after #include <system.h> in you source file:
//
//#define LCD_ARGS 	2,	/* Interface type: mode 0 = 8bit, 1 = 4bit(low nibble), 2 = 4bit(upper nibble) */ \
//		1, 				/* Use busy signal: 1 = use busy, 0 = use time delays */\
//		PORTB, TRISB, 	/* Data port and data port tris register */ \
//		PORTA, TRISA, 	/* Control port and control port tris register */ \
//		3,				/* Bit number of control port is connected to RS */ \
//		2,				/* Bit number of control port is connected to RW */ \
//		1 				/* Bit number of control port is connected to Enable */
//
//#include <lcd_driver.h> // include the LCD template code
//
//
// Revisions
// =========
//
// V1.10 David Hobday 03/03/2005
// =============================
// 1) Improved documentation in file
// 2) Changed delays to delay_10us for usage on target with clock <= 4MHz
// 3) Changed template to make more friendly and obvious
// 
// V1.11 18/03/2005
// David Hobday
// 1) Fixed operation with PIC18 - template arguments of incorrect type
// 2) Tested with PicDem2Plus board (4MHz PIC16F877/PIC18F452)
// 3) Added lcd_gotoxy function.
// 4) Added option to use display busy bit or time delays
// 5) Added overloaded lprintf function for ROM string
// 6) Added overloaded lprintf function to output numbers supported formats:
//  "%d" - decimal
//  "%X" - hex
//  "%b" - binart
//  example: display binary number six digits, '0' as file character.
//  lprintf( "val:%06b", numb );
// 7) Other improvements/cleanup.
//
//
// V1.11 David Hobday 25/03/2005
// =============================
// 1) Fixed bug with lprintf( "test:%d", 0 ); not printing a 0.
// 2) Added a few more comments in lprintf( const char*, int ) code
// 
//
// V1.12 Jason Sobell 03/07/2005
// =============================
// 1) Fixed display of unsigned integers >32767
//
//
// V1.13 David Hobday 08/07/2005
// =============================
// 1) Removed code that is no longer needed (as val is now unsigned).
// 2) Tweaked Jason Sobell changes a little to reduce code size.
//
// 
// V1.14 David Hobday 11/11/2005
// =============================
// 1) Changed lcd_gotoxy function so that it works as expected with 20 x 4 display
//

////////////////////////////////////////////////////////////////////////////

// Section added by TJW to facilitate use by multiple modules.

// Standardized on putting constants here.
#include "lcd_consts.h"

// Changed this section to be accessible in a companion C module like normal C,
// so this module can be used in real-world multi-module projects.
#ifdef IN_LCD
char writeDelayType;
#else
extern char writeDelayType;
#endif

// Added these convenience functions and defines.

// Additional commands.
#define set_cg_ram				0x40 // First charcter - add the character offset, 0-7.

// Defines a custom character at code point c, with data in s.
// Works specifically for 5x7 characters.
// c must be in the range 0-7.
// s must be 7 bytes long.
// See http://www.quinapalus.com/hd44780udg.html for a good way to generate the data.
void lcd_set_char(unsigned char c, rom char* s);


////////////////////////////////////////////////////////////////////////////
// LCD Commands ( Refer to LCD Data Sheet )
////////////////////////////////////////////////////////////////////////////
#define clear_lcd         		0x01 // Clear Display
#define return_home       		0x02 // Cursor to Home position
#define entry_mode        		0x06 // Normal entry mode
#define entry_mode_rev    		0x04 // Normal entry mode  -reverse direction
#define entry_mode_scroll 		0x07 // - with shift
#define entry_mode_scroll_rev	0x05 // reverse direction

#define system_set_8_bit  		0x38 // 8 bit data mode 2 line ( 5x7 font )
#define system_set_4_bit  		0x28 // 4 bit data mode 2 line ( 5x7 font )
#define system_set_reset  		0x30 // Reset code
#define display_on        		0x0C // Display ON - 2 line mode
#define display_off       		0x08 // Display off
#define set_dd_line1      		0x80 // Line 1 position 1
#define set_dd_line2      		0xC0 // Line 2 position 1
#define set_dd_ram        		0x80 // Line 1 position 1
#define write_data        		0x00 // With RS = 1
#define cursor_on         		0x0E // Switch Cursor ON
#define cursor_off        		0x0C // Switch Cursor OFF
#define cursor_blink_on   		0x0F // Cursor plus blink
#define cursor_shift_right		0x14 // Move cursor right
#define cursor_shift_left 		0x10 // Move cursor left
#define display_shift_right		0x1C // Scroll display right
#define display_shift_left		0x18 // Scroll display left

#define WriteNoDelay	1
#define WriteControlled	0

// Interface type
#define LCD_8_BIT_MODE 0
#define LCD_4_BIT_LO_NIB_MODE 1
#define LCD_4_BIT_HI_NIB_MODE 2

// These macros make susequent code more readable, but can seem a little confusing
#define _LCD_RawWriteNibble LCD_RawWriteNibble	<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_RawWriteNibbleInline  LCD_RawWriteNibbleInline	<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_Read			LCD_Read			<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_WaitForNotBusy	LCD_WaitForNotBusy	<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_Write			LCD_Write			<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_FunctionMode	LCD_FunctionMode	<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_DataMode		LCD_DataMode		<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_RawWrite		LCD_RawWrite		<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>
#define _LCD_ClockOut		LCD_ClockOut		<InterfaceType, UseBusy, DataPort, Data_PortTris, CtrlPort, Ctrl_PortTris, RS, RW, E>

#define _LCD_TEMPL		template <	unsigned char InterfaceType,\
									unsigned char UseBusy,\
									unsigned int DataPort, unsigned int Data_PortTris,\
									unsigned int CtrlPort, unsigned int Ctrl_PortTris,\
									unsigned char RS, unsigned char RW, unsigned char E>

_LCD_TEMPL
inline void LCD_FunctionMode( void )
{
	volatile bit rs@CtrlPort.RS = 0;
}

_LCD_TEMPL
inline void LCD_DataMode( void )
{
	volatile bit rs@CtrlPort.RS = 1;
}


inline void LCD_CycleMakeupDelay()
{
	// Enable cycle time must be > 1000ns total for both reading and writing
	// LCD_SetupDelay + LCD_EnablePulseDelay + LCD_HoldupDelay + LCD_CycleMakeupDelay >= 1000ns
	//       200      +          500         +       100       +          200         >= 1000ns

	// This delay is required to meet the Sharp data sheet total cycle time of > 1000ns
	// @40MHz this is 2 instructions
	asm nop 
	asm nop	
}


inline void LCD_EnablePulseDelay()
{		
	// PWEH > 460ns on Sharp data sheet
	// @40MHz this is 5 instructions
	asm nop 
	asm nop
	asm nop
	asm nop
	asm nop
}

inline void LCD_SetupDelay()
{
	// tAS > 140ns min on Sharp data sheet
	// @40MHz this is 2 instructions
	asm nop 
	asm nop
}

inline void LCD_HoldupDelay()
{
	// tAS > 10ns min on Sharp data sheet
	// @40MHz this is 1 instructions
	asm nop
}



_LCD_TEMPL
char LCD_Read()
{		
	char d; 
	volatile unsigned char data@DataPort, tris@Data_PortTris;
	volatile bit rw@CtrlPort.RW, e@CtrlPort.E;
	
	if( InterfaceType == LCD_4_BIT_HI_NIB_MODE )
	{
		// upper nibble input
		tris |= 0xF0;
		rw = 1; // set reading mode
		// first high nibble	
		LCD_SetupDelay();
		e = 1;
		LCD_EnablePulseDelay();
		d = data & 0xF0; // read data
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
			
		// then low nibble
		LCD_SetupDelay();
		e = 1;
		LCD_EnablePulseDelay();
		d |= data >> 4;
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}


	if( InterfaceType == LCD_4_BIT_LO_NIB_MODE )
	{
		// lower nibble input	
		tris |= 0x0F;
		rw = 1; // set reading mode
		// first high nibble
		LCD_SetupDelay();
		e = 1;
		LCD_EnablePulseDelay();
		d = data << 4;
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
			
		// then low nibble
		LCD_SetupDelay();
		e = 1;
		LCD_EnablePulseDelay();	
		d |= data & 0x0F;
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}


	if( InterfaceType == LCD_8_BIT_MODE )
	{
		// port input	
		tris = 0xFF;
		rw = 1; // set reading mode
		LCD_SetupDelay();
		e = 1;
		LCD_EnablePulseDelay();
		d = data;
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}
	
	return d;
}

_LCD_TEMPL
void LCD_RawWriteNibble( char d )
{
	// Note: this function is duplicate below, but declared inline.
	// this is to reduce stack depth usage
	// Note: this function is above, but declared inline.
	// this is to reduce stack depth usage
	volatile unsigned char data@DataPort, tris@Data_PortTris;
	volatile bit rw@CtrlPort.RW, e@CtrlPort.E;
	
	if( InterfaceType == LCD_4_BIT_HI_NIB_MODE )
	{
		// port upper nibble output
		rw = 0; // set writing mode
		LCD_SetupDelay();
		tris &= 0x0F;
		data &= 0x0F;
		data |= d & 0xF0;
		e = 1;
		LCD_EnablePulseDelay();
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}

	if( InterfaceType == LCD_4_BIT_LO_NIB_MODE )
	{
		// port upper nibble output
		rw = 0; // set writing mode
		LCD_SetupDelay();
		tris &= 0xF0;
		data &= 0xF0;
		data |= d >> 4;
		e = 1;
		LCD_EnablePulseDelay();
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}	
}

_LCD_TEMPL
inline void LCD_RawWriteNibbleInline( char d )
{
	// Note: this function is above, but declared inline.
	// this is to reduce stack depth usage
	volatile unsigned char data@DataPort, tris@Data_PortTris;
	volatile bit rw@CtrlPort.RW, e@CtrlPort.E;
	
	if( InterfaceType == LCD_4_BIT_HI_NIB_MODE )
	{
		// port upper nibble output
		rw = 0; // set writing mode
		LCD_SetupDelay();
		tris &= 0x0F;
		data &= 0x0F;
		data |= d & 0xF0;
		e = 1;
		LCD_EnablePulseDelay();
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}

	if( InterfaceType == LCD_4_BIT_LO_NIB_MODE )
	{
		// port upper nibble output
		rw = 0; // set writing mode
		LCD_SetupDelay();
		tris &= 0xF0;
		data &= 0xF0;
		data |= d >> 4;
		e = 1;
		LCD_EnablePulseDelay();
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}	
}


_LCD_TEMPL
void LCD_RawWrite( char d )
{		
	volatile unsigned char tris@Data_PortTris, data@DataPort;
	volatile bit rw@CtrlPort.RW, e@CtrlPort.E;
		
	if( InterfaceType == LCD_4_BIT_HI_NIB_MODE  )
	{
		// output upper nibble, then lower nibble
		bit flag = 0;
		do
		{			
			_LCD_RawWriteNibbleInline( d );
			flag = !flag;
			d <<= 4;
		}
		while( flag );
	}	

	if( InterfaceType == LCD_4_BIT_LO_NIB_MODE )
	{
		// output upper nibble, then lower nibble
		bit flag = 0;
		do
		{			
			_LCD_RawWriteNibbleInline( d );
			flag = !flag;
			d <<= 4;
		}
		while( flag );
	}	


	if( InterfaceType == LCD_8_BIT_MODE )
	{
		// port b output
		rw = 0; // set writing mode
		tris = 0x00;		
		data = d;
		LCD_SetupDelay();
		e = 1;
		LCD_EnablePulseDelay();
		e = 0;
		LCD_HoldupDelay();
		LCD_CycleMakeupDelay();
	}
}

_LCD_TEMPL
inline void LCD_WaitForNotBusy()
{
	volatile bit rs@CtrlPort.RS; bit old_RS = rs;	
	rs = 0;
	while( _LCD_Read() & 0x80 ); // wait while busy set
	rs = old_RS;
}

_LCD_TEMPL
void LCD_Write(char d )
{
	volatile bit rs@CtrlPort.RS;
	
	if( UseBusy == 1 )
	{	
		// wait until display Not busy before sending next data
		if ( writeDelayType == WriteControlled )
			_LCD_WaitForNotBusy();
		
		_LCD_RawWrite( d );
	}
	else
	{		
 		_LCD_RawWrite( d );
		
		// give time to complete
		if ( writeDelayType == WriteControlled )
		{
			if( !rs && (d == return_home || d == clear_lcd) )
				delay_ms( 2 ); // return_home takes more time than other instructions to execute
			else
				delay_10us( 5 ); // 50us - enough time for normal command execution - clear and home need longer!!			
		}
	}
}

_LCD_TEMPL
void LCD_Clear()
{
	_LCD_FunctionMode();
	_LCD_Write( clear_lcd ); // clear display
	_LCD_Write( return_home );
}

_LCD_TEMPL
void LCD_Setup( void )
{
	// set control port bits used to output
	volatile bit trisRS@Ctrl_PortTris.RS, trisRW@Ctrl_PortTris.RW, trisE@Ctrl_PortTris.E;
	trisRS = 0;
	trisRW = 0;
	trisE = 0;
	
	writeDelayType = WriteNoDelay; // no delays in data writes
	
	delay_ms(16); // Power up delay
	_LCD_FunctionMode();

	if( InterfaceType == LCD_4_BIT_HI_NIB_MODE )
	{
		// Reset sequence as described in data sheets
		_LCD_RawWriteNibble( system_set_reset ); 
		delay_ms(5); // min delay here of 4.1 ms
		_LCD_RawWriteNibble( system_set_reset );
		delay_10us(100); // min delay here of 100us
		_LCD_RawWriteNibble( system_set_reset );
	   
		// LCD busy flag is valid from this point onwards
		if( UseBusy == 1 )
			_LCD_WaitForNotBusy();
		else
			delay_10us( 5 ); // standard command delay time
		
		_LCD_RawWriteNibble( system_set_4_bit );
		
		if( UseBusy == 1 )
			_LCD_WaitForNotBusy();
		else
			delay_10us( 5 ); // standard command delay time

		writeDelayType = WriteControlled;
		_LCD_Write( system_set_4_bit );
	}

	if( InterfaceType == LCD_4_BIT_LO_NIB_MODE )
	{
		// Reset sequence as described in data sheets
		_LCD_RawWriteNibble( system_set_reset ); 
		delay_ms(5); // min delay here of 4.1 ms
		_LCD_RawWriteNibble( system_set_reset );
		delay_10us(100); // min delay here of 100us
		_LCD_RawWriteNibble( system_set_reset );
	   
		// LCD busy flag is valid from this point onwards
		if( UseBusy == 1 )
			_LCD_WaitForNotBusy();
		else
			delay_10us( 5 ); // standard command delay time
		
		_LCD_RawWriteNibble( system_set_4_bit );
		
		if( UseBusy == 1 )
			_LCD_WaitForNotBusy();
		else
			delay_10us( 5 ); // standard command delay time

		writeDelayType = WriteControlled;
		_LCD_Write( system_set_4_bit );
	}
	
	if( InterfaceType == LCD_8_BIT_MODE )
	{
		// Reset sequence as described in data sheets
		_LCD_RawWrite( system_set_reset ); 
		delay_ms(5); // min delay here of 4.1 ms
		_LCD_RawWrite( system_set_reset );
		delay_10us(10); // min delay here of 100us
		_LCD_RawWrite( system_set_reset );
	   
		// busy flag is valid from this point onwards
		if( UseBusy == 1 )
			_LCD_WaitForNotBusy();
		else
			delay_10us( 5 ); // standard command delay time

		_LCD_RawWrite( system_set_8_bit );	

		if( UseBusy == 1 )
			_LCD_WaitForNotBusy();
		else
			delay_10us( 5 ); // standard command delay time
		
		writeDelayType = WriteControlled; // use busy
	}
		
	_LCD_Write( entry_mode );
	_LCD_Write( display_on );
	_LCD_Write( set_dd_ram );
}

_LCD_TEMPL
void LCD_Printf( const char *lcdptr )
{
	char pi = 0, c;
	_LCD_DataMode();
    while( 1 )
    {
		c = lcdptr[pi++];
		if ( !c )
			return;
		if ( c == '\n' )
		{
			_LCD_FunctionMode();
			// move to start second line
			_LCD_Write( set_dd_ram + 0x40 );
			_LCD_DataMode();
		}
		else
			_LCD_Write( c );// Display on LCD
	}
}

_LCD_TEMPL
void LCD_Printf( rom char *lcdptr )
{
	char pi = 0, c;
	_LCD_DataMode();
    while( 1 )
    {
		c = lcdptr[pi++];
		if ( !c )
			return;
		if ( c == '\n' )
		{
			_LCD_FunctionMode();
			// move to start second line
			_LCD_Write( set_dd_ram + 0x40 ); 
			_LCD_DataMode();
		}
		else
			_LCD_Write( c );// Display on LCD
	}
}

_LCD_TEMPL
void LCD_Printf( const char *lcdptr, unsigned int val ) // JS - Accept unsigned by default
{
	unsigned char pi = 0, bi, c, fill, baseOrBits, sign, mask;
	unsigned char buff[ 10 ]; // max length allow is 9
	bit pad;
	
	_LCD_DataMode();
    while( 1 )
    {
		c = lcdptr[pi++]; if ( !c ) return;
		
		switch( c )
		{
		case '\n':
			_LCD_FunctionMode();
			// move to start second line
			_LCD_Write( set_dd_ram + 0x40 );
			_LCD_DataMode();
			break;
		case '%':
			c = lcdptr[pi++]; if ( !c ) return;
			
			//Handle escape sequence that prints '%'
			if ( c == '%' )
			{
				_LCD_Write( c );// Display on LCD
				break;
			}
			
			// Next character if zero indicates that we should zero fill output
			if ( c == '0' )
			{
				fill = '0';
				c = lcdptr[pi++]; if ( !c ) return;
			}
			else
				fill = ' ';

			// Next character if valid digit indicates field width
			if( c > '0' && c <= '9' )
			{
				pad = 1;
				bi = c - 48;;				
				c = lcdptr[pi++]; if ( !c ) return;
			}
			else
			{
				pad = 0;
				bi = sizeof( buff ) - 1;
			}
				
			
			// Next character indicates the radix (number base)
			sign = 0;
			switch( c )
			{
			case 'd':
				if( val & 0x8000 )	// Negative values must be adjusted to be positive // JS
				{
					sign = '-';
					val ^= 0xFFFF; // 2s complement negate	// JS
					val++;
				}
			case 'u':
				baseOrBits = 10; // base ten, divide by ten per digit
				break;			
			case 'X':
				baseOrBits = 4; // base 16, requires a 4 bit shift per digit
				mask = 0x0F;
				break;
			case 'b':
				baseOrBits = 1; // base 16, requires a 1 bit shift per digit
				mask = 0x01;
				break;
			default:
				return; // no radix
			}
				
			// null terminate, then reverse fill string
			buff[ bi ] = '\0';
			
			bit first = true;				
			while( bi )
			{
				bi--;
				if( val || first )
				{
					first = false;
										
					if( baseOrBits == 10 )
					{
						c = (unsigned char)(val % 10);	// JS - Optimization, use absolute of 10
						val /= 10;	// JS - Optimization, use absolute of 10
					}
					else
					{
						c = val & mask;
						val = ((unsigned int)val) >> baseOrBits;						
					}
					
					if( c > 9 )
						c += 55; // convert to hex digits character A-F
					else
						c += 48; // convert to digit character 0-9

				}
				else
				{
					if( sign && (bi == 0 || fill != '0') )
					{
						c = sign;
						sign = 0;
					}
					else
						c = fill;
				}
				
				buff[ bi ] = c;
				
				if( pad == 0 && val == 0 && sign == 0 )
					break;
			}
			// output string to display
			while( 1 )
			{
				c = buff[ bi ];
				if( !c ) break;
				_LCD_Write( c );// Display on LCD
				bi++;
			}
			break;
		default:
			_LCD_Write( c );// Display on LCD
			break;
		}
	}
}

_LCD_TEMPL
void LCD_GotoXy( char x, char y )
{
	// displays memory mapping with two lines:
	// line 1: 0x00
	// line 2: 0x40
	
	// display memory mapping with four lines:
	// line 1: 0x00
	// line 2: 0x40
	// line 3: 0x14
	// line 4: 0x54
	
	_LCD_FunctionMode();
	unsigned char offset = x;
	if( y & 0x01 ) offset += 0x40;
	if( y & 0x02 ) offset += 0x14;		
	_LCD_Write( set_dd_ram + offset );
}


_LCD_TEMPL
void LCD_Function( char func )
{
	_LCD_FunctionMode();
	_LCD_Write( func );
}

////////////////////////////////////////////////////////////////////////////
// Helpers that hide template arguments
////////////////////////////////////////////////////////////////////////////
// low level functions
#define lcd_write		LCD_Write<LCD_ARGS>
#define lcd_waitfornotbusy LCD_WaitForNotBusy<LCD_ARGS>
#define lcd_read		LCD_Read<LCD_ARGS>
#define lcd_funcmode	LCD_FunctionMode<LCD_ARGS>
#define lcd_datamode	LCD_DataMode<LCD_ARGS>


// high level functions - these all set function or data mode as required
#define lcd_setup		LCD_Setup<LCD_ARGS>
#define lprintf			LCD_Printf<LCD_ARGS>
#define lcd_clear		LCD_Clear<LCD_ARGS>
#define lcd_gotoxy		LCD_GotoXy<LCD_ARGS>
#define lcd_function 	LCD_Function<LCD_ARGS>
