/* log.c
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
    Provides various ways to compute logarithms.
    
    Portions based on an algorithm described by Scott Dattalo at:
		http://www.dattalo.com/technical/theory/logs.html
    which in turn references, and is a refinement of:
		Knuth, Donald E., "The Art of Computer Programming Vol 1", Addison-Wesley Publishing Company, ISBN 0-201-03822-6.
*/

#include <system.h>
#include "fixed16.h"

#include "log.h"

rom unsigned char* log_table = {
	0x00,  // 0x100*log2(0x100/0x100)
	0x16,  // 0x100*log2(0x110/0x100)
	0x2b,  // 0x100*log2(0x120/0x100)
	0x3f,  // 0x100*log2(0x130/0x100)
	0x52,  // 0x100*log2(0x140/0x100)
	0x64,  // 0x100*log2(0x150/0x100)
	0x76,  // 0x100*log2(0x160/0x100)
	0x86,  // 0x100*log2(0x170/0x100)
	0x96,  // 0x100*log2(0x180/0x100)
	0xa5,  // 0x100*log2(0x190/0x100)
	0xb3,  // 0x100*log2(0x1a0/0x100)
	0xc1,  // 0x100*log2(0x1b0/0x100)
	0xcf,  // 0x100*log2(0x1c0/0x100)
	0xdc,  // 0x100*log2(0x1d0/0x100)
	0xe8,  // 0x100*log2(0x1e0/0x100)
	0xf4,  // 0x100*log2(0x1f0/0x100)
	0x00,  // 0x100*log2(0x200/0x100)
};

fixed16 log2_us(unsigned short x)
{
	unsigned char g = 15; 
	while (g > 0 && !(x & 0x8000))   // loop until msb of x occupies bit 15
	{
		x <<= 1; 
		g--;
	}
	
	// If g==0, then we're done.
	if (g == 0) 
		return 0;
	
	// We have the integer portion of the log(x). Now get the fractional part.
	x <<= 1;                    // "normalize" x, get rid of the MSB.
	unsigned char j = x >> 12;  // Get the array index
	unsigned short x_minus_a = x & 0xfff;      
		// The lower bits of x are all that's left after subtracting "a".
		
	unsigned char gf = log_table[j];
	gf += (x_minus_a * (log_table[j + 1] - gf)) >> 12;
	
	fixed16 result;
	MAKE_FIXED(result, g, gf);
	return result;
}

fixed16 log2_f(fixed16 x)
{
	if (x < 1)
		// No error trapping is really available here, so return the smallest possible log for 0 or negative numbers.
		return 0;
	else
		return log2_us(x) - FIXED_FROM_BYTE(8);
}

#ifdef TEST_LOG
void main(void)
{
	// log2(2) = 1 = 0x0100
	fixed16 x = FIXED_FROM_BYTE(2);
	fixed16 L = log2_f(x);
	
	// log2(1) = 0 = 0x0000
	x = 0;
	L = log2_f(x);
	
	// log2(8) = 3 = 0x0300
	x = FIXED_FROM_BYTE(8);
	L = log2_f(x);
	
	// log2(3.20) = 1.676 = 0x01AD
	x = makeFixed(3, 0x33);
	L = log2_f(x);
	
	// log2(6.94) = 2.793 = 0x02CB
	x = makeFixed(6, 0xF1);
	L = log2_f(x);
	
	// log2(18.33) = 4.195 = 0x0431
	x = makeFixed(18, 0x55);
	L = log2_f(x);
	
	// log2(0.93) = -0.105 = 0xFFE6
	x = makeFixed(0, 0xEE);
	L = log2_f(x);
	
	// log2(0.80) = -0.328 = 0xFFAC
	x = makeFixed(0, 0xCD);
	L = log2_f(x);
	
	// log2(12.48) = 3.641 = 0x03A4
	x = makeFixed(12, 0x7A);
	L = log2_f(x);
	
	// log2(0) = 0, as defined
	x = 0;
	L = log2_f(x);
	
	// log2(-1) = 0, as defined
	x = FIXED_FROM_BYTE(-1);
	L = log2_f(x);
	
	x = 0;
}
#endif