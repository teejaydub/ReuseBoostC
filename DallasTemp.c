/* DallasTemp.c
    Copyright (c) 2006-2008 by Timothy J. Weber, tw@timothyweber.org.

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

#include "crc_8bit.h"
#include "onewire.h"
#include "types-tjw.h"

#include "DallasTemp.h"


// Defines for 1-Wire commands.
#define DT_ConvertT  0x44
#define DT_ReadScratchPad  0xBE
#define DT_WriteScratchPad  0x4E
#define DT_CopyScratchPad  0x48
#define DT_ReadPowerSupply  0xB4
	
// offset of the resolution bits in the config register
#define DT_ConfigResOffset  5

// The number of significant bits for low-res and high-res sensing.
#define BitsToSense_LowRes  9
#define BitsToSense_HighRes  12

// Conversion time in ms, for low-res sensing.
#define ConversionTime_LowRes  95
#define ConversionTime_HighRes  751


unsigned char DT_CountSensors(byte bus)
{
	// For now, assume we have at most one sensor connected.
	if (OWB_Reset(bus))
		return 1;
	else
		return 0;
}

byte DT_IsParasite(byte bus)
{
	if (bus) {
		OW_SendByte_2(OW_SkipROM);
		OW_SendByte_2(DT_ReadPowerSupply);
		return !OW_ReadBit_2();  // parasite-powered devices pull the bus low.
	} else {
		OW_SendByte(OW_SkipROM);
		OW_SendByte(DT_ReadPowerSupply);
		return !OW_ReadBit();  // parasite-powered devices pull the bus low.
	}
}

void StartRead(byte bus, unsigned char configReg)
{
	byte useParasite = DT_IsParasite(bus);
	
	if (bus) {
		// Talk to whoever's attached (assuming one).
		OW_SendByte_2(OW_SkipROM);
		
		// Configure for low resolution.
		OW_SendByte_2(DT_WriteScratchPad);
		OW_SendByte_2(DT_MAX_TEMP);  // Alarm THigh (disable).
		OW_SendByte_2(DT_MIN_TEMP - 1);  // Alarm TLow (disable).
		OW_SendByte_2(configReg);  // Resolution.
		
		OW_Reset_2();  // Don't need to write anything else.
		
		// Start temperature conversion.
		OW_SendByte_2(OW_SkipROM);
		OW_SendByte_2(DT_ConvertT);
		
		// Support parasite power.
		if (useParasite) 
			OW_PowerOn_2();
	} else {
		// Talk to whoever's attached (assuming one).
		OW_SendByte(OW_SkipROM);
		
		// Configure for low resolution.
		OW_SendByte(DT_WriteScratchPad);
		OW_SendByte(DT_MAX_TEMP);  // Alarm THigh (disable).
		OW_SendByte(DT_MIN_TEMP - 1);  // Alarm TLow (disable).
		OW_SendByte(configReg);  // Resolution.
		
		OW_Reset();  // Don't need to write anything else.
		
		// Start temperature conversion.
		OW_SendByte(OW_SkipROM);
		OW_SendByte(DT_ConvertT);
		
		// Support parasite power.
		if (useParasite) 
			OW_PowerOn();
	}
}

signed short DoRead(byte bus, unsigned char configReg, unsigned short conversionTime)
{
	StartRead(bus, configReg);
	
	// Wait for it to finish.
	while (conversionTime > 255) {
		delay_ms(255);
		conversionTime -= 255;
	}
	delay_ms((unsigned char)(conversionTime));
	
	return DT_GetLastTemp(0);
}

signed char DT_ReadTempRough(byte bus)
{
	// The bus can only be 0 or 1.
	if (bus > 1)
		return DT_MIN_TEMP;
		
	// Check for the sensor one more time.
	if (!OWB_Reset(bus))
		return 0;
		
	short value = DoRead(bus, 
		(BitsToSense_LowRes - 9) << DT_ConfigResOffset, 
		ConversionTime_LowRes);

	// Construct the return value from the two value bytes.
	signed char result = value >> 8;
	
	unsigned char lsb;
	LOBYTE(lsb, value);
	
	if (lsb.7)  // 2^-1 bit
		// round up
		return result + 1;
	else
		// round down
		return result;
}

signed short DT_ReadTempFine(byte bus)
{
	// The bus can only be 0 or 1, and it must have something on it.
	if (bus > 1 || !OWB_Reset(bus))
		return 0;
	else 
		return DoRead(bus, 
			(BitsToSense_HighRes - 9) << DT_ConfigResOffset, 
			ConversionTime_HighRes);
}

unsigned char DT_StartReadFine(byte bus)
{
	// The bus can only be 0 or 1, and it must have something on it.
	if (bus > 1 || !OWB_Reset(bus))
		return 0;
	else { 
		StartRead(bus, (BitsToSense_HighRes - 9) << DT_ConfigResOffset);
		return 1;
	}
}

unsigned char DT_ReadDone(byte bus)
{
	return OWB_ReadByte(bus);
}

fixed16 DT_GetLastTemp(byte bus)
{
	// Read the temperature value.
	if (!OWB_Reset(bus))
		return DT_BAD_TEMPERATURE;

	unsigned char lsb;
	signed char msb;
		
	byte b;

	if (bus) {
		// Bus #2.
		OW_SendByte_2(OW_SkipROM);
		OW_SendByte_2(DT_ReadScratchPad);
		
		lsb = OW_ReadByte_2();
		msb = OW_ReadByte_2();
		
	// Check CRC.
		
		// Start with what we've already read.
		crc8Init();
		crc8(lsb);
		crc8(msb);
	
		// Read and include the next 6 bytes.
		
		#ifdef TEMP_DIAGS
			// @2-3 = user byte, doesn't matter
			crc8(OW_ReadByte_2());
			crc8(OW_ReadByte_2());
			
			// @4 = Configuration, 0x1F bits should be on.
			b = OW_ReadByte_2();
			crc8(b);
			
			if ((b & 0x1F) != 0x1F)
				return 0x0A00;  // = 50 F
			
			// @5 = Reserved (0xFF)
			b = OW_ReadByte_2();
			crc8(b);
			
			if (b != 0xFF)
				return 0x0480;  // = 40 F
			
			// @6 = Reserved (0xOC, but apparently varies)
			b = OW_ReadByte_2();
			crc8(b);
			
			// @7 = Reserved (0x10)
			b = OW_ReadByte_2();
			crc8(b);
			
			if (b != 0x10)
				return 0x0CC;  // = 55 F
		#else
			crc8(OW_ReadByte_2());
			crc8(OW_ReadByte_2());

			// @4 = Configuration, 0x1F bits should be on.
			b = OW_ReadByte_2();
			if ((b & 0x1F) != 0x1F)
				return DT_BAD_TEMPERATURE;
			crc8(b);

			crc8(OW_ReadByte_2());
			crc8(OW_ReadByte_2());
			crc8(OW_ReadByte_2());
		#endif
			
		// Byte @8 is the CRC itself.
		if (OW_ReadByte_2() != crc)
			// Didn't pass the test.
			#ifdef TEMP_DIAGS
				return 0x2300;  // = 95 F
			#else
				return DT_BAD_TEMPERATURE;
			#endif
	} else {
		// Bus #1.
		OW_SendByte(OW_SkipROM);
		OW_SendByte(DT_ReadScratchPad);
		
		lsb = OW_ReadByte();
		msb = OW_ReadByte();

	// Check CRC.
		
		// Start with what we've already read.
		crc8Init();
		crc8(lsb);
		crc8(msb);
	
		crc8(OW_ReadByte());
		crc8(OW_ReadByte());

		// @4 = Configuration, 0x1F bits should be on.
		b = OW_ReadByte();
		if ((b & 0x1F) != 0x1F)
			return DT_BAD_TEMPERATURE;
		crc8(b);

		crc8(OW_ReadByte());
		crc8(OW_ReadByte());
		crc8(OW_ReadByte());
			
		// Byte @8 is the CRC itself.
		if (OW_ReadByte() != crc)
			// Didn't pass the test.
			return DT_BAD_TEMPERATURE;
	}
		
	// Adjust to a sane representation.
	fixed16 result = makeFixed(msb, lsb);
	result <<= 4;
	
	return result;
}

