/* serial.c
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
	Routines for software serial support.
	Supports 9600 baud, 8/N/1, receive-only, on pin RB7.
	(Could be parameterized, but on the 16F627/628/648 must be on PORTB for interrupt-on-change.)
*/

#define IN_SERIAL

#include <system.h>

#include "serial.h"
#include "serial-consts.h"


#ifdef SOFTWARE_RECEIVE
	
	#define RECEIVE_PIN  7
	bit receiveBit@PORTB.RECEIVE_PIN;
	
	char bitsRemaining;  // to be read, including stop bit
	unsigned char dataIn;  // the byte we're in the process of reading; not yet complete.
	
	#define BAUD_RATE  9600
	#define CLOCK_FREQ  4000000
	#define CYCLE_RATE  CLOCK_FREQ / 4
	
	// one baud period, in cycles
	#define BAUD_PERIOD  ((unsigned char) (CYCLE_RATE / BAUD_RATE))
	
	// 1.5 baud periods, in cycles
	#define INTRO_BAUD_PERIOD  ((unsigned char) (CYCLE_RATE * 3 / 2 / BAUD_RATE))

#endif

// The input queue.
#ifdef SOFTWARE_RECEIVE
	// Currently only one byte.
	unsigned char dataQueue;
#else
	// A FIFO queue, growing forward in memory.
	// Head is the first element added; Tail is the next one to be added.
	// Head == Tail when empty.
	// (Tail + 1) mod n == Head when full.
	unsigned char dataQueue[SERIAL_QUEUE_LENGTH];
	unsigned char* dataQueueHead;
	unsigned char* dataQueueTail;
	unsigned char* queueNextTail;
	const unsigned char* queueEnd = dataQueue + SERIAL_QUEUE_LENGTH;  // one past the end
	
#endif

void InitializeSerial()
{
	InitializeSerial2(true, false);
}

void InitializeSerial2(bool useReceive, bool useTransmit)
{
	// Initialize globals.
	ser_hasData = 0;
	ser_error = 0;
	
	// Stuff for both receive and transmit.
	
	#ifdef SOFTWARE_RECEIVE
	
	#else

		// Set for 9600 baud.
		txsta.BRGH = 1;  // Set baud rate to...
		spbrg = 25;  // ... 9600 baud
		rcsta.SPEN = 1;  // Enable serial port.
		
	#endif

	// Transmit-only stuff.
	
	if (useTransmit) {
		txsta.TXEN = 1;
	}
		
	// Receive-only stuff.
	
	if (useReceive) {
	
	#ifdef SOFTWARE_RECEIVE
		
		// Initialize module-locals.
		bitsRemaining = 0;
		
		// Set up RB4..7 for interrupt-on-change.
		intcon.RBIE = 1;
		intcon.RBIF = 0;
		
		// Set up Timer 2 for 9600 baud.
		// Ideally, that's every 104 instructions.
		// So we need no pre- or post-scaling for a one-byte counter.
		t2con.TMR2ON = 0;  // ...but don't start it yet.
		
		intcon.TMR2IF = 0;
		intcon.TMR2IE = 0;
		intcon.PEIE = 1;
	
	#else
		
		rcsta.CREN = 1;  // Enable serial transmission.
		pie1.RCIE = 1;  // Enable interrupts on reception.
		
		intcon.PEIE = 1;
		
		dataQueueHead = dataQueueTail = dataQueue;
		queueNextTail = dataQueueTail + 1;
	
	#endif
	}
	
	// Enable interrupts.
	intcon.GIE = 1;
}

void SerialInterrupt()
{
	#ifdef SOFTWARE_RECEIVE
		
		// First, try to continue a byte that's already been started.
		if (pir1.TMR2IF) {
			if (!ser_error && bitsRemaining) {
				// Shift the next bit in from the left.
				if (--bitsRemaining > 0)
					// Rotate receivePin into dataIn.
					asm {
						bcf _status, C
						btfsc _portb, RECEIVE_PIN
						bsf _status, C
						rrf _dataIn, F
					}
				else
					// This is the stop bit.
					if (!receiveBit) {
						// But it wasn't set - must be a framing error.
						ser_error = 1;
						ser_errorType = 'F';
					}
				
				// If we're out of bits, we've received the entire byte.
				// (Because the first 1 we'll encounter was the start bit we shifted in.)
				if (!bitsRemaining && !ser_error) {
					// Done, and no error.
					// Return it.
					if (ser_hasData) {
						// Collision error.
						ser_error = 1;
						ser_errorType = 'C';
					}
						
					dataQueue = dataIn;
					ser_hasData = 1;
				} else {
					// Otherwise, just leave the timer running for the next baud period.
					pr2 = BAUD_PERIOD;
				}
			}
			
			pir1.TMR2IF = 0;
		} else if (intcon.RBIF) {
			if (!ser_error && !receiveBit) {
				// No byte in progress, but one seems to have just started.
				
				// Set up to receive 8 more data bits and 1 stop bit.
				bitsRemaining = 9;
				
				// Delay one and a half baud periods before the next interrupt,
				// so we synchronize in the middle of an incoming bit.
				pr2 = INTRO_BAUD_PERIOD;
				tmr2 = 36 + 34;  // (rough compensation for the delay in getting this far + the delay in processing the first bit)
				t2con.TMR2ON = 1;
			}
			
			// Clear the interrupt.
			intcon.RBIF = 0;  // should already be done when receiveBit is read.
		}
				
		// If we're done, set the interrupt on pin change;
		// if not, we can just keep the timer interrupt on.
		// It's one or the other - we never need both.
		// Another way to think of it is that the IOC is a way to save
		// processor cycles (though it also allows us to react closer 
		// to the leading edge of the start bit).
		if (ser_error) {
			// Disable both our interrupts.
			intcon.TMR2ON = 0;
			intcon.RBIE = 0;
			intcon.RBIF = 0;
		} else if (!bitsRemaining) {
			// Done for the moment.  Just watch for the pin to change.
			intcon.RBIE = 1;
			intcon.RBIF = 0;
			intcon.TMR2IE = 0;
			t2con.TMR2ON = 0;
		} else {
			// Not yet done.  Poll the bit at the next timer interrupt.
			intcon.RBIE = 0;
			pir1.TMR2IF = 0;
			pie1.TMR2IE = 1;
			t2con.TMR2ON = 1;
		}
	
	#else
	// !SOFTWARE_RECEIVE
	
		if (pir1.RCIF && !ser_error) {
			if (rcsta.FERR) {
				ser_errorType = 'F';
				ser_error = 1;
				ser_hasData = 0;
			} else if (rcsta.OERR) {
				ser_errorType = 'C';
				ser_error = 1;
				ser_hasData = 0;
			} else if (queueNextTail == dataQueueHead) {  // queue is full
				ser_error = 1;
				ser_errorType = 'c';
			} else {
				*dataQueueTail = rcreg;
				
				dataQueueTail = queueNextTail;
				
				if (++queueNextTail == queueEnd)
					queueNextTail = dataQueue;
					
				ser_error = 0;
			}

			ser_hasData = !ser_error;
		}
		
	#endif
}

unsigned char ReadSerial()
{
	unsigned char result;
#ifdef SOFTWARE_RECEIVE
	result = dataQueue;
	ser_hasData = 0;
#else
	result = *dataQueueHead;
	
	// Increment and handle rollover.
	if (++dataQueueHead == queueEnd)
		dataQueueHead = dataQueue;
		
	ser_hasData = (dataQueueHead != dataQueueTail);
#endif
	return result;
}
