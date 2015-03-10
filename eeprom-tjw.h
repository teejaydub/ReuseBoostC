/* eeprom-tjw.h
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
*/

// Portable EEPROM read and write routines.
// Addresses start at 0 for the first byte of EEPROM.

char read_eeprom(char addr);
void write_eeprom(char addr, char data);

void read_eeprom_block(char addr, char* buf, unsigned char len);
void write_eeprom_block(char addr, char* buf, unsigned char len);

// Adds one to an unsigned long counter at the given EEPROM address.
// Avoids overflow by bracketing the count at its max value.
void inc_eeprom_counter_long(char addr);

// Accumulates the maximum unsigned value in the given EEPROM address.
void accum_max_eeprom_byte(char addr, unsigned char value);

// EE_PIR is the register where EEIF is.
#if defined(_PIC16F688)
	#define EE_PIR pir1
#else
	#define EE_PIR pir2
#endif

// Wait for a previous EEPROM write to finish.
// Must have been preceded by an EEPROM write call.
inline void wait_eeprom_write(void)
{	
	// Wait for the write to complete.
	while (!EE_PIR.EEIF)
		clear_wdt();
}