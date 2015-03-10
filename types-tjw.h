/* types-tjw.h
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

#ifndef _TJW_TYPES
#define _TJW_TYPES

// Unsigned chars are used all the time -- cut down on typing.
typedef unsigned char byte;

// Defines for normal and active-low logic.
#define TRUE_ACTIVE_LOW  0
#define FALSE_ACTIVE_LOW  1

// Returns a bitmask for the specified bit number.
#define BITMASK(bitNum)  (1 << bitNum)

// Sets the specified bit in the specified register based on the given expression.
inline void setBit(char& reg, byte bitNum, byte testValue)
{
	if (testValue) 
		reg |= BITMASK(bitNum); 
	else 
		reg &= (~BITMASK(bitNum)); 
}

// Toggles the specified bit in the specified register.
#define toggleBit(reg, bitNum)  (reg ^= BITMASK(bitNum))

#endif
