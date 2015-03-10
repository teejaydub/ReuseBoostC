/*
	CRC-8 for Dallas iButton products
	
	
	From Maxim/Dallas AP Note 27
	
	"Understanding and Using Cyclic Redundancy Checks with 
	Dallas Semiconductor iButton Products"
	
	The Ap note describes the CRC-8 algorithm used in the 
	iButton products.
	
	18JAN03 - T. Scott Dattalo
	
	Modified by Timothy Weber.
*/

#ifndef __CRC_8BIT_H
#define __CRC_8BIT_H

#ifdef IN_CRC_8BIT
 #define CRC_8BIT_EXTERN
#else
 #define CRC_8BIT_EXTERN  extern
#endif


// Holds the last-returned CRC.
// Used as input to the next one.
CRC_8BIT_EXTERN unsigned char crc;

// Call this before distinct runs of CRC values.
inline void crc8Init(void)  { crc = 0; }

// Returns the CRC of the given byte, preceded by all bytes since crcInit().
unsigned char crc8(unsigned char data);


#endif
//__CRC_8BIT_H