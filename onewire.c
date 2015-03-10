/* onewire.c
    Copyright (c) 2006-2007 by Timothy J. Weber, tw@timothyweber.org.

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

#include "onewire.h"
#include "onewire-const.h"


#define OUTPUT_LOW  { clear_bit(ow_port_, OW_PIN); ow_port = ow_port_; clear_bit(ow_tris, OW_PIN); }
#define OUTPUT_LOW_2  { clear_bit(ow_port_, OW_PIN_2); ow_port = ow_port_; clear_bit(ow_tris, OW_PIN_2); }

#define OUTPUT_HIGH  { set_bit(ow_port_, OW_PIN); ow_port = ow_port_; clear_bit(ow_tris, OW_PIN); }
#define OUTPUT_HIGH_2  { set_bit(ow_port_, OW_PIN_2); ow_port = ow_port_; clear_bit(ow_tris, OW_PIN_2); }

#define OUTPUT_HIZ  set_bit(ow_tris, OW_PIN)
#define OUTPUT_HIZ_2  set_bit(ow_tris, OW_PIN_2)


char OW_Reset()
{
	// Disable interrupts.
	intcon.GIE = 0;

	// Low for T_RSTL >= 480 us.
	OUTPUT_LOW;
	
	delay_10us(50);
	
	// Hi-Z.
	OUTPUT_HIZ;
	
	// Wait until halfway between the end of the earliest, shortest possible presence detect pulse (75 us)
	// and the beginning of the latest possible one (60 us).
	// That's 67.5, so wait 60 + a few cycles.
	// But we'll go with 70, since that's what the Dallas docs use.
	delay_10us(7);
	
	// Test the result.
	char result = ow_port.OW_PIN == 0;
	
	// Interrupts are OK now.
	intcon.GIE = 1;
	
	// Allow it to complete for the remainder of T_RSTH = 480 us since Hi-Z.
	// 480 - 60 = 420.
	// The docs say 430, so use that.
	delay_10us(43);
	
	OUTPUT_HIGH;
	
	return result;
}

void OW_SendByte(unsigned char b)
{
	unsigned char bitCount = 8;
	
	// Disable interrupts.
	intcon.GIE = 0;

	sendLoop:
		// Low for 4 us (docs say 5 us).
		nop();
		nop();
		OUTPUT_LOW;
		nop();
		nop();
		
		// Output the next bit.
		asm { 
			rrf _b, F 
		}
		if (status.C)
			set_bit(ow_port, OW_PIN);
			
		// Wait for 60 us.
		delay_10us(6);
		
		// Recovery time >= 1 us.
		OUTPUT_HIGH;
		
		nop();
		nop();
		
		asm {
			decfsz _bitCount, F
			goto sendLoop
		}
	
	// Restore interrupts.
	intcon.GIE = 1;
	
	OUTPUT_HIGH;
}

byte OW_ReadBit()
{
	byte result = 0;
	
	// Disable interrupts.
	intcon.GIE = 0;
	
		// Low for 6 us.
		nop();
		nop();
		OUTPUT_LOW;
		nop();
		nop();
		nop();
		nop();

		// HIZ for 4 us.
		OUTPUT_HIZ;
		nop();
		nop();
		nop();
		nop();
		nop();
		
		// Get the next bit.

//		if (OW_PORT.OW_PIN)  // this should happen <= 15 ms from when the output goes low, but close to it.
//			set_bit(result, 7);
//		else
//			clear_bit(result, 7);
#if OW_MASK == 0x01
		asm { 
			movf _ow_port, W
			andlw 0x01  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK == 0x04
		asm { 
			movf _ow_port, W
			andlw 0x04  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK == 0x08
		asm { 
			movf _ow_port, W
			andlw 0x08  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK == 0x10
		asm { 
			movf _ow_port, W
			andlw 0x10  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#else
 #error "Have to hard-code the pin constant here, unfortunately."
#endif
		
		// Total time must be > 61 us.
		delay_10us(5);
	
	// Restore interrupts.
	intcon.GIE = 1;
	
	OUTPUT_HIGH;
	
	return result;
}

unsigned char OW_ReadByte()
{
	unsigned char bitCount = 8;
	unsigned char result = 0;
	
	// Disable interrupts.
	intcon.GIE = 0;

	recLoop:
		// Low for 6 us.
		nop();
		nop();
		OUTPUT_LOW;
		nop();
		nop();
		nop();
		nop();

		// HIZ for 4 us.
		OUTPUT_HIZ;
		nop();
		nop();
		nop();
		nop();
		nop();
		
		// Get the next bit.

//		if (OW_PORT.OW_PIN)  // this should happen <= 15 ms from when the output goes low, but close to it.
//			set_bit(result, 7);
//		else
//			clear_bit(result, 7);
#if OW_MASK == 0x01
		asm { 
			movf _ow_port, W
			andlw 0x01  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK == 0x04
		asm { 
			movf _ow_port, W
			andlw 0x04  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK == 0x08
		asm { 
			movf _ow_port, W
			andlw 0x08  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK == 0x10
		asm { 
			movf _ow_port, W
			andlw 0x10  // OW_MASK
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#else
 #error "Have to hard-code the pin constant here, unfortunately."
#endif
		
		// Total time must be > 61 us.
		delay_10us(5);
	
		asm {
			decfsz _bitCount, F
			goto recLoop
		}
	
	// Restore interrupts.
	intcon.GIE = 1;
	
	OUTPUT_HIGH;
	
	return result;
}

void OW_PowerOn()
{
	OUTPUT_HIGH;
}

//=============================================================================
// Functions for the second bus.
// Identical to the first, except they use all the _2 macros.

char OW_Reset_2()
{
	// Disable interrupts.
	intcon.GIE = 0;

	// Low for T_RSTL >= 480 us.
	OUTPUT_LOW_2;
	
	delay_10us(25);
	delay_10us(25);
	
	// Hi-Z.
	OUTPUT_HIZ_2;
	
	// Wait until halfway between the end of the earliest, shortest possible presence detect pulse (75 us)
	// and the beginning of the latest possible one (60 us).
	// That's 67.5, so wait 60 + a few cycles.
	// But we'll go with 70, since that's what the Dallas docs use.
	delay_10us(7);
	
	// Test the result.
	char result = ow_port.OW_PIN_2 == 0;
	
	// Interrupts are OK now.
	intcon.GIE = 1;
	
	// Allow it to complete for the remainder of T_RSTH = 480 us since Hi-Z.
	// 480 - 60 = 420.
	// The docs say 430, so use that.
	delay_10us(20);
	delay_10us(23);
	
	OUTPUT_HIGH_2;

	return result;
}

void OW_SendByte_2(unsigned char b)
{
	unsigned char bitCount = 8;
	
	// Disable interrupts.
	intcon.GIE = 0;

	sendLoop2:
		// Low for 4 us (docs say 5 us).
		nop();
		nop();
		OUTPUT_LOW_2;
		nop();
		nop();
		
		// Output the next bit.
		asm { 
			rrf _b, F 
		}
		if (status.C)
			set_bit(ow_port, OW_PIN_2);
			
		// Wait for 60 us.
		delay_10us(6);
		
		// Recovery time >= 1 us.
		OUTPUT_HIGH_2;
		
		nop();
		nop();
		
		asm {
			decfsz _bitCount, F
			goto sendLoop2
		}
	
	// Restore interrupts.
	intcon.GIE = 1;

	OUTPUT_HIGH_2;
}

byte OW_ReadBit_2()
{
	byte result = 0;
	
	// Disable interrupts.
	intcon.GIE = 0;
	
		// Low for 6 us.
		nop();
		nop();
		OUTPUT_LOW_2;
		nop();
		nop();
		nop();
		nop();

		// HIZ for 4 us.
		OUTPUT_HIZ_2;
		nop();
		nop();
		nop();
		nop();
		nop();
		
		// Get the next bit.

//		if (OW_PORT.OW_PIN_2)  // this should happen <= 15 ms from when the output goes low, but close to it.
//			set_bit(result, 7);
//		else
//			clear_bit(result, 7);
#if OW_MASK_2 == 0x20
		asm { 
			movf _ow_port, W
			andlw 0x20  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK_2 == 0x04
		asm { 
			movf _ow_port, W
			andlw 0x04  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK_2 == 0x08
		asm { 
			movf _ow_port, W
			andlw 0x08  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK_2 == 0x10
		asm { 
			movf _ow_port, W
			andlw 0x10  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#else
 #error "Have to hard-code the pin constant here, unfortunately."
#endif
		
		// Total time must be > 61 us.
		delay_10us(5);
	
	// Restore interrupts.
	intcon.GIE = 1;
	
	OUTPUT_HIGH_2;

	return result;
}

unsigned char OW_ReadByte_2()
{
	unsigned char bitCount = 8;
	unsigned char result = 0;
	
	// Disable interrupts.
	intcon.GIE = 0;

	recLoop2:
		// Low for 6 us.
		nop();
		nop();
		OUTPUT_LOW_2;
		nop();
		nop();
		nop();
		nop();

		// HIZ for 4 us.
		OUTPUT_HIZ_2;
		nop();
		nop();
		nop();
		nop();
		nop();
		
		// Get the next bit.

//		if (OW_PORT.OW_PIN_2)  // this should happen <= 15 ms from when the output goes low, but close to it.
//			set_bit(result, 7);
//		else
//			clear_bit(result, 7);
#if OW_MASK_2 == 0x20
		asm { 
			movf _ow_port, W
			andlw 0x20  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK_2 == 0x04
		asm { 
			movf _ow_port, W
			andlw 0x04  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK_2 == 0x08
		asm { 
			movf _ow_port, W
			andlw 0x08  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#elif OW_MASK_2 == 0x10
		asm { 
			movf _ow_port, W
			andlw 0x10  // OW_MASK_2
			addlw 255  // C = 1 iff the next input bit = 1
			rrf _result, F
		}
#else
 #error "Have to hard-code the pin constant here, unfortunately."
#endif
		
		// Total time must be > 61 us.
		delay_10us(5);
	
		asm {
			decfsz _bitCount, F
			goto recLoop2
		}
	
	// Restore interrupts.
	intcon.GIE = 1;
	
	OUTPUT_HIGH_2;

	return result;
}

void OW_PowerOn_2()
{
	OUTPUT_HIGH_2;
}

//=============================================================================
// Bus-parameterized functions.

char OWB_Reset(byte bus)
{
	if (bus)
		return OW_Reset_2();
	else
		return OW_Reset();
}

void OWB_SendByte(byte bus, unsigned char b)
{
	if (bus)
		OW_SendByte_2(b);
	else
		OW_SendByte(b);
}

byte OWB_ReadByte(byte bus)
{
	if (bus)
		return OW_ReadByte_2();
	else
		return OW_ReadByte();
}

byte OWB_ReadBit(byte bus)
{
	if (bus)
		return OW_ReadBit_2();
	else
		return OW_ReadBit();
}

void OWB_PowerOn(byte bus)
{
	if (bus)
		OW_PowerOn_2();
	else
		OW_PowerOn();
}
