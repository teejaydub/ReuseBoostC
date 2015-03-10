/* onewire.h
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
/*
	Software support for Dallas 1-Wire protocol, as master.
	
	To use, define constants in onewire-const.h.
	
	And, be sure to activate either internal or external pullup on the pin.
	
	Supports up to two busses; the second bus is accessed using the *_2 functions.
	(This is primarily for coding and testing ease; it would probably be tighter
	and not significantly slower to parameterize the bus dynamically.)
	
	To maintain timing requirements, interrupts are disabled during bus reads and writes,
	so this module can't coexist with something that requires real-time interrupts.
*/

#include "types-tjw.h"
	
// Defines for 1-Wire commands

#define OW_SearchROM  0x0F0
#define OW_ReadROM  0x33
#define OW_MatchROM  0x55
#define OW_SkipROM  0xCC


// Resets the bus.
// Returns true if there's a presence on the bus.
char OW_Reset();
char OW_Reset_2();

// Sends a byte to the bus.
void OW_SendByte(unsigned char b);
void OW_SendByte_2(unsigned char b);

// Reads a byte from the bus.
byte OW_ReadByte();
byte OW_ReadByte_2();

// Reads a single bit from the bus.
// Returns nonzero if it's one, zero if it's zero.
byte OW_ReadBit();
byte OW_ReadBit_2();

// Drives power to the bus until the next operation.
void OW_PowerOn();
void OW_PowerOn_2();

// Bus-parameterized versions.
// Bus can be 0 or 1, currently.
char OWB_Reset(byte bus);
void OWB_SendByte(byte bus, unsigned char b);
byte OWB_ReadByte(byte bus);
byte OWB_ReadBit(byte bus);
void OWB_PowerOn(byte bus);
