/* chipinit.h
    Copyright (c) 2007-2011 by Timothy J. Weber, tw@timothyweber.org.

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


	Standard code to initialize PICs.
	
	Packaged in inline functions, because it plays easier with the compilers
	(can't specify source files in other directories!)
	and because this code is typically only used once, so inline doesn't cost anything.
	
	As I use new chips, I'll add chip-specific sections.
	
	Chips supported:
	
	PIC12F629
	PIC12F675
	PIC12F683
	PIC16F688
	PIC16F690
	PIC16F916
	PIC18F2320
	PIC18F2550
	PIC18F2620
*/

#if defined(_PIC12F675) || defined(_PIC16F916) || defined(_PIC16F688) || defined(_PIC12F683) 
#elif defined(_PIC18F2320) || defined(_PIC18F2620) || defined(_PIC16F690) || defined(_PIC16F883) || defined(_PIC16F886)
#elif defined(_PIC18F1320) || defined(_PIC18F2550)
	// New chips supported must be listed here.
#else
	#error "chipinit.h: Chip not recognized; check code and add this chip's required settings"
#endif

// Disables all optional peripherals, including:
//  Comparators
//  A/D converters
//  LCD driver
inline void DisablePeripherals(void)
{
	// Disable comparators.
	#if defined(_PIC12F629) || defined(_PIC12F675) || defined(_PIC18F2620) || defined(_PIC18F2550)
		cmcon = 7;
	#endif
	
	#if defined(_PIC16F916) || defined(_PIC16F688) || defined(_PIC12F683)
		cmcon0 = 7;
	#endif
	
	#if defined(_PIC16F883) || defined(_PIC16F886)
		// These are the start-up defaults.
		//cm1con0 = 0; 
		//cm2con0 = 0;
	#endif
	
	// Disable A/D.
	#if defined(_PIC12F675) || defined(_PIC16F916) || defined(_PIC16F688) || defined(_PIC12F683) || defined(_PIC16F690) || defined(_PIC16F883) || defined(_PIC16F886)
		ansel = 0;
	#endif
	
	#if defined(_PIC16F690) || defined(_PIC16F883) || defined(_PIC16F886)
		anselh = 0;
	#endif

	#if defined(_PIC18F2320) || defined(_PIC18F2620) || defined(_PIC18F1320) || defined(_PIC18F2550)
		adcon1 = 0x0F;  // all digital inputs	
	#endif
	
	// Disable LCD driver.
	#if defined(_PIC16F916)
		lcdcon.VLCDEN = 0;
	#endif
}
