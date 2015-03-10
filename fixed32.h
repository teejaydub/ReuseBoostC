/* fixed32.h
    Copyright (c) 2009 by Timothy J. Weber, tw@timothyweber.org.

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
	Defines a type for 32-bit fixed point numbers with 16 integral and 16 fractional bits.

	Only bare-bones support so far - just what's needed and timely for CoolBot.
	
	Rules for using fixed-point variables:
	
	- Convert integers and fractions to fixed-point variables using the various 
		conversion routines.
	
	- You can add and subtract two fixed-point values directly.
	
	- You can multiply two fixed-point values together and get a fixed result 
		by right-shifting 16 bits.  The shift can take place on ONE of the two values beforehand,
		or on the result afterwards. 
		
	- Similarly, you can divide two fixed-point values and get a fixed result
		by left-shifting the dividend (top) 16 bits, or by left-shifting 
		the result 16 bits, or by right-shifting the divisor (bottom) 16 bits.
		
	- These shifts can also take place partly before and partly after,
		to preserve resolution when you know ahead of time the maximum magnitude
		of one of the operands.
		
	- You can multiply a fixed-point with a plain integer (any size)
		directly - it's like you've already done the right-shift.
		The result is fixed-point.
		
	- Similarly, you can divide a fixed-point by a plain integer directly
		to get a fixed-point result.
*/

#ifndef __FIXED32_H
#define __FIXED32_H

#include "fixed16.h"


typedef signed long fixed32;


// Returns b converted to fixed-point, with no fractional part.
inline fixed32 fixed32FromShort(signed short b)
{
	return ((fixed32) b) << 16;
}
// This version is more efficient.
#define FIXED32_FROM_SHORT(b)  (b << 16)

// Returns b as a fixed-point fraction, with no integral part.
inline fixed32 fixed32FracFromShort(unsigned short b)
{
	return (fixed32) b;
}
// This version is more efficient.
#define FIXED32_FRAC_FROM_SHORT(b)  (b)

inline fixed32 makeFixed32(signed short integral, unsigned short fractional)
{
	return (((fixed32) integral) << 16) | ((fixed32) fractional);
}
// This version is more efficient.
#define MAKE_FIXED32(result, integral, fractional)  (((fixed32) integral) << 16) | ((fixed32) fractional)
#define MAKE_FIXED32_CONST(integral, fractional)  (integral * 256 + fractional)

inline fixed32 fixed32FromFixed16(fixed16 f)
{
	return ((fixed32) f) << 8;
}

// Rounds the fractional part.
inline fixed16 fixed16FromFixed32(fixed32 f)
{
	return (fixed16) ((f + 128) >> 8);
}

// Returns the integral part of f in i.
inline void fixed32IntegralTo(fixed32 f, signed short& i)
{
	i = (signed short) (f >> 16);
}
// This version is more efficient.
#define FIXED32_INTEGRAL_TO(f, i)  ((unsigned short) (f >> 16))

// Returns the fractional part of f times 65536.
inline void fixed32FracTo(fixed32 f, unsigned short& frac)
{
	frac = (unsigned short) (f & 0xFFFF);
}
// This version is more efficient.
#define FIXED32_FRAC_TO(f, frac)  ((unsigned short) (f & 0xFFFF))

// Return f truncated to an integer.
inline signed short fixed32Integral(fixed32 f)
{
	return f >> 16;
}
// More efficient.
#define FIXED32_INTEGRAL(f)  (f >> 16)

// Returns f truncated to an integer.
inline fixed32 fixedTrunc(fixed32 f)
{
	return f & 0xFFFF0000;
}

// Returns true if f has a nonzero fractional part.
inline char fixed32HasFrac(fixed32 f)
{
	return (f & 0x0000FFFF) != 0;
}

// Return f rounded to the nearest integer.
inline signed short fixed32RoundToByte(fixed32 f)
{
	return (signed short)((f + 0x00008000) >> 16);
}

// Return 1/f.
inline fixed32 fixed32Reciprocal(fixed32 f)
{
	return ((1 << 16) / f) << 16;
}

#endif
