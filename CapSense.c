/* CapSense.c
	Using capacitive touch sensing to simulate up to 4 pushbuttons.
	Follows the guidelines set out in Microchip's AN1101 and AN1103.
*/

#define IN_CAPSENSE

#include <system.h>
#include <memory.h>
#include <stdlib.h>

#include "eeprom-tjw.h"
#include "math-tjw.h"

#include "CapSense.h"
#include "CapSense-consts.h"

 
CapSenseReading csReadings[MAX_CAPSENSE_CHANNELS];

// The channel currently being timed by the hardware.
byte currentCapSenseChannel;

// The baseline value from which we expect low-going excursions when a finger approaches.
CapSenseReading csBaseline[MAX_CAPSENSE_CHANNELS];

#ifdef CS_AUTO_CALIBRATE
// These are kept here during calibration; cleared by the calibration code.

// The min reading for each channel seen during the current state.
CapSenseReading csMin[MAX_CAPSENSE_CHANNELS];
#endif

// This is set if a button was pressed during the current bin - includes held down.
byte csDownInBin[MAX_CAPSENSE_CHANNELS];

#define TICKS_PER_BIN_CHANGE  1
byte csLastBinTicks;

byte csLastButtonTicks;
byte csLastDownPolls;  // Set to zero when any button is down, incremented when no button is down, up to 255.

// Set to one of the channels when that button is down, or NO_CAPSENSE_BUTTONS if none are down.
// Will often still be down after GetCapSenseButton() has cleared csButton.
byte csHoldingButton;

// Holds the index of the most-recently-pressed button, or NO_CAPSENSE_BUTTONS
// if none have been pressed since the last call to GetCapSenseButton().
byte csButton;


//==================================================================
// Main code

inline void SetCapSenseChannel(void)
{
	// In addition to selecting the channel,
	// these vaules connect the comparators to the right voltage references,
	// and set their outputs to the inputs of the SR latch.
	cm1con0 = 0x94 + currentCapSenseChannel;
	
	// This also sets comparator's 2 output to appear on the C2OUT pin, which
	// is routed to charge and discharge all of the sensors in parallel.
	cm2con0 = 0xA0 + currentCapSenseChannel;
}

inline void RestartCapSenseTimer(void)
{
	// Clear out and reset both timers.
	// Timer 1 will start counting oscillations afresh,
	// and Timer 0 will restart its count to 256 cycles till the interrupt.
	tmr0 = 0;
	t1con.TMR1ON = 0;
	tmr1l = 0;
	tmr1h = 0;
	t1con.TMR1ON = 1;
	intcon.T0IF = 0;
}

// Returns true if the channel with the specified index is used, as defined by the constants in CapSense-consts.h.
byte IsChannelUsed(byte channel)
{
	return (1 << channel) & CAPSENSE_CHANNELS;
}

void InitCapSense(void)
{
	#ifndef _PIC16F886
	 #error "Need to determine register usage for this algorithm used with this chip."
	#endif
	
	// Set up the relaxation oscillator.
	// Values taken from Appendix A of Microchip AN1101.
	cm2con1 = 0x32;
	srcon = 0xF0;
	vrcon = 0x8D;  // Enable the voltage reference, in the low range, as 21/32 of Vdd.
	
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL0
	ansel.0 = 1;  // on RA0, AN0
	trisa.0 = 1;
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL1
	ansel.1 = 1;  // on RA1, AN1
	trisa.1 = 1;
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL2
	anselh.1 = 1;  // on RB3, AN9
	trisb.3 = 1;
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL3
	anselh.2 = 1;  // on RB1, AN10
	trisb.1 = 1;
	#endif

	// Set current to the first one we're using.
	currentCapSenseChannel = FIRST_CAPSENSE_CHANNEL;

	// The low voltage reference is always used, and is on RA2, AN2.
	ansel.2 = 1;
	trisa.2 = 1;
	
	// The SR latch outputs on RA5, which is also C2OUT.
	trisa.5 = 0;
	
	// RC0 is T1CKI, which must be hard-wired to C2OUT externally.
	trisc.0 = 1;
	
	// Timer 1 takes its input from the T1CKI pin.
	t1con.TMR1CS = 1;
	
	// Set up the interrupt on TMR0 overflow.
	// It runs free, and we check TMR1's value on each TMR0 overflow interrupt.
	InitUiTime_Timer0();
	
	// Clear all bins.
	csCurrentBin = 0;
	memset(csBin, 0, sizeof(csBin)); 
	memset(csBaseline, 0, sizeof(csBaseline));  // Set to zero to prevent any presses until we've had time to stabilize.
	memset(csReadings, 0, sizeof(csReadings));
	csLastBinTicks = ticks;
	csLastDownPolls = 255;
	csHoldingButton = NO_CAPSENSE_BUTTONS;
	memset(csDownInBin, 0, sizeof(csDownInBin));
	
	read_eeprom_block(CAPSENSE_EEPROM_ADDR, (char*) csThresholds, CAPSENSE_EEPROM_LEN);

	SetCapSenseChannel();
	RestartCapSenseTimer();
	
	csButton = NO_CAPSENSE_BUTTONS;
}

CapSenseReading GetLastCapSenseReading(byte index)
{
	return csReadings[index];
}

CapSenseReading GetBaseline(byte index)
{
	return csBaseline[index];
}

byte GetCapSenseButton(void)
{
	byte result = csButton;
	csButton = NO_CAPSENSE_BUTTONS;
	return result;
}

inline void BumpCapSenseBin(void)
{
	if (++csCurrentBin >= NUM_CAPSENSE_BINS)
		csCurrentBin = 0;
	
	// Find the global max again, over all bins, for all channels.
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL0
	csBaseline[0] = max(csBin[0][0], csBin[0][1]);
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL1
	csBaseline[1] = max(csBin[1][0], csBin[1][1]);
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL2
	csBaseline[2] = max(csBin[2][0], csBin[2][1]);
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL3
	csBaseline[3] = max(csBin[3][0], csBin[3][1]);
	#endif

	// Reset each channel's newly-current bin to contain just that channel's most-recent reading.
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL0
	csBin[0][csCurrentBin] = csReadings[0];
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL1
	csBin[1][csCurrentBin] = csReadings[1];
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL2
	csBin[2][csCurrentBin] = csReadings[2];
	#endif
	#if CAPSENSE_CHANNELS & CAPSENSE_CHANNEL3
	csBin[3][csCurrentBin] = csReadings[3];
	#endif
	
	csLastBinTicks = ticks;
	csDownInBin[csCurrentBin] = false;
}

inline void BumpCapSenseChannel(void)
{
	currentCapSenseChannel++;
	
	// Skip unused channels.
	#if !(CAPSENSE_CHANNELS & CAPSENSE_CHANNEL1)
	if (currentCapSenseChannel == 1)
		currentCapSenseChannel++;
	#endif
	#if !(CAPSENSE_CHANNELS & CAPSENSE_CHANNEL2)
	if (currentCapSenseChannel == 2)
		currentCapSenseChannel++;
	#endif
	#if !(CAPSENSE_CHANNELS & CAPSENSE_CHANNEL3)
	if (currentCapSenseChannel == 3)
		currentCapSenseChannel++;
	#endif
	
	if (currentCapSenseChannel > LAST_CAPSENSE_CHANNEL)
		currentCapSenseChannel = FIRST_CAPSENSE_CHANNEL;
		
	SetCapSenseChannel();

	// Start the next reading.
	RestartCapSenseTimer();
}

byte CapSenseISR(void)
{
	if (UiTimeInterrupt()) {
		// Read TMR1.
		CapSenseReading reading = (tmr1h << 8) | tmr1l;
		
		// Do some of the indexing once.
		CapSenseReading* currentBaseline = &csBaseline[currentCapSenseChannel];
		CapSenseReading* currentReading = &csReadings[currentCapSenseChannel];
		
		// Compute the "pressed" threshold.
		// threshold = the threshold constant - sensitivity, but bracketed at the minimum threshold.
		CapSenseReading threshold = *currentBaseline;
		byte sensitivity = csThresholds[currentCapSenseChannel];
		if (threshold > sensitivity) {
			threshold -= sensitivity;
			threshold = max(threshold, CS_MIN_THRESHOLD);
		} else
			threshold = CS_MIN_THRESHOLD;
		
		// Filter the new value.
		// Running average, over FILTER_LENGTH samples.
		reading = *currentReading + (reading - *currentReading) / FILTER_LENGTH;
		*currentReading = reading;

		// Is it a button press?
		if (reading < threshold) {
			// Yes, it's "down."
			if (csButton == NO_CAPSENSE_BUTTONS && csHoldingButton == NO_CAPSENSE_BUTTONS
				&& csLastDownPolls > DEBOUNCE_POLLS  // debounce by number of polls - ~<= 1000/sec.
			) {
				// And this is the falling edge: note it.
				csButton = currentCapSenseChannel;
				csLastButtonTicks = ticks;
				csHoldingButton = currentCapSenseChannel;
				csLastDownPolls = 0;
				csDownInBin[csCurrentBin] = true;
			}
		} else {
			// No, it's not "down".
			// If this is the button we were holding, we're not holding it anymore.
			if (csHoldingButton == currentCapSenseChannel)
				csHoldingButton = NO_CAPSENSE_BUTTONS;
				
			if (csHoldingButton == NO_CAPSENSE_BUTTONS && csLastDownPolls < 255)
				++csLastDownPolls;
		}
	
		// Update the current bin's extreme.
		// But not if this button is down... 
		CapSenseReading* currentMax;
		if ((csHoldingButton == NO_CAPSENSE_BUTTONS && csLastDownPolls > DEBOUNCE_POLLS)
			// ... until it's been down for a really long time.
			|| (ticks - csLastButtonTicks) >= 0 * TICKS_PER_SEC
		) {
			//accumulateMax<CapSenseReading>(&csBin[currentCapSenseChannel][csCurrentBin], reading);
			// That works, but the resulting function call uses one too many stack levels.
			currentMax = &csBin[currentCapSenseChannel][csCurrentBin];
			if (reading > *currentMax)
				*currentMax = reading;
		}
#ifdef CS_AUTO_CALIBRATE
		// During calibration, keep track of the minima as well.
		if (csAutoCalibrateState == acPressAndReleaseButton)
			accumulateMin<CapSenseReading>(&csMin[currentCapSenseChannel], reading);
#endif		
		
		// Move to the next min bin, every other tick (about twice a second).
		if (ticks - csLastBinTicks >= TICKS_PER_BIN_CHANGE)
			BumpCapSenseBin();
	
		// Move to the next sensor.
		BumpCapSenseChannel();
	
		return true;
	} else
		return false;
}

void CapSenseISRDone(void)
{
	if (intcon.T0IF)
		// See the app notes: This means another interrupt has occurred before we returned from the last one.
		// That means the current count is inaccurate, so it must be discarded.
		RestartCapSenseTimer();
}


//==================================================================
// Calibration

#ifdef CS_AUTO_CALIBRATE

#define SETTLE_TICKS  (TICKS_PER_BIN_CHANGE * NUM_CAPSENSE_BINS + 1)  // Allow time for all bins to be overwritten.
byte ticksStateStart;

#define TIMES_THRU_BUTTONS  3
byte timesThruButtons;

#ifdef CS_AUTO_CALIBRATE
// The maximum depression from the baseline seen while the button is considered to be released and pressed, respectively.
// Positive values represent negative deviations from the baseline.
CapSenseReading csMaxWaiting[MAX_CAPSENSE_CHANNELS];
CapSenseReading csMaxHolding[MAX_CAPSENSE_CHANNELS][TIMES_THRU_BUTTONS];

// Ditto, but while another button is being pressed.
CapSenseReading csMaxOthers[MAX_CAPSENSE_CHANNELS];
#endif


CapSenseReading csMinOtherButtons[MAX_CAPSENSE_CHANNELS];  // min of each button while other buttons are being pressed

// Sets all elements in the array of length count to value.
void InitReadingArray(CapSenseReading* array, byte count, CapSenseReading value)
{
	while (count--)
		*array++ = value;
}

inline void EnterState(CSAutoCalibrateState newState)
{
	csAutoCalibrateState = newState;
	ticksStateStart = ticks;
}

void CapSenseStartCalibrate(void)
{
	EnterState(acStart);
}

byte CapSenseContinueCalibrate(void)
{
	byte channel;
	
	// Call this to ensure buttons are taken out of the one-element "queue",
	// simulating normal processing.
	GetCapSenseButton();
	
	// State actions and transitions.
	
	switch (csAutoCalibrateState) {
	
	case acStart:
		csCalButton = FIRST_CAPSENSE_CHANNEL;
		timesThruButtons = 0;
		InitReadingArray(csMaxWaiting, MAX_CAPSENSE_CHANNELS, 0);
		InitReadingArray(csMaxHolding, MAX_CAPSENSE_CHANNELS * TIMES_THRU_BUTTONS, 0);
		InitReadingArray(csMaxOthers, MAX_CAPSENSE_CHANNELS, 0);
		EnterState(acPressNothing);
		break;
		
	case acPressNothing:
		if (ticks - ticksStateStart > SETTLE_TICKS) {
			// Done waiting.
			// Move mins into csMinWaiting.
			for (channel = FIRST_CAPSENSE_CHANNEL; channel <= LAST_CAPSENSE_CHANNEL; ++channel)
				accumulateMax<CapSenseReading>(&csMaxWaiting[channel], csBaseline[channel] - csMin[channel]);
			
			// Move to the first button, if this is the first "nothing down" period.
			if (timesThruButtons == 0) {
				EnterState(acPressAndReleaseButton);
			} else {
				// Otherwise, we're done.
				EnterState(acDone);
			}
		}
		break;
		
	case acPressAndReleaseButton:
		if (ticks - ticksStateStart > SETTLE_TICKS) {
			// Done waiting.
			// Move the from csMin into csMinHolding and csMinOthers. 
			for (channel = FIRST_CAPSENSE_CHANNEL; channel <= LAST_CAPSENSE_CHANNEL; ++channel) {
				CapSenseReading* accumulator;
				if (channel == csCalButton)
					// Accumulate the min for the current button into the "min while holding" accumulator.
					accumulator = &csMaxHolding[channel][timesThruButtons];
				else
					// Accumulate the min for other channels while this button is being pressed, over all trials.
					accumulator = &csMaxOthers[channel];
	
				// Get it from the min bin, which already contains a min of many recent readings.
				accumulateMax<CapSenseReading>(accumulator, csBaseline[channel] - csMin[channel]);
			}
		
			// Move to the next button.
			EnterState(acPressAndReleaseButton);
			++csCalButton;
			
			// Skip over inactive channels.
			while (!IsChannelUsed(csCalButton) && csCalButton <= LAST_CAPSENSE_CHANNEL)
				++csCalButton;

			// Reset the csMin for this button and pass.
			InitReadingArray(csMin, MAX_CAPSENSE_CHANNELS, MAX_CS_READING);

			// Start a new round, if we've been through all channels.
			if (csCalButton > LAST_CAPSENSE_CHANNEL) {
				csCalButton = FIRST_CAPSENSE_CHANNEL;
				++timesThruButtons;
				
				if (timesThruButtons >= TIMES_THRU_BUTTONS)
					EnterState(acPressNothing);
			} 
		}
		break;
	}
	
	if (csAutoCalibrateState == acDone) {
		// csMaxWaiting, csMaxHolding, and csMaxOthers have been collected.
		// Finish computing the target thresholds and rate the results.
		for (csCalButton = FIRST_CAPSENSE_CHANNEL; csCalButton <= LAST_CAPSENSE_CHANNEL; ++csCalButton) {
			if (IsChannelUsed(csCalButton)) {
				// Find the maximum excursions (lowest deviation from baseline) for this channel.
				CapSenseReading maxWaiting = csMaxWaiting[csCalButton];
				CapSenseReading maxOthers = csMaxOthers[csCalButton];
							
				// Find the minimum and maximum readings during *this* button's presses.
				CapSenseReading minMe = MAX_CS_READING;  // the smallest maximum excursion for the weakest press
				CapSenseReading maxMe = 0;  // the largest excursion for the strongest press
				byte i;
				for (i = 0; i < TIMES_THRU_BUTTONS; ++i) {
					minMe = min(minMe, csMaxHolding[csCalButton][i]);
					maxMe = max(maxMe, csMaxHolding[csCalButton][i]);
				}
				
				// Report a Fail status if we can't tell the difference between a neighboring button and our own,
				if (maxOthers + 2 * CS_MIN_THRESHOLD >= minMe 
					// or between our lightest press and our noisiest non-press.
					|| maxWaiting + 2 * CS_MIN_THRESHOLD >= minMe)
				{ 
					csResults[csCalButton] = acrFail;
					
					// But, still calculate a threshold based on telling this button's hardest press apart from its release.
					// The buttons may fight, but at least we can tell when one of them is hit.
					csThresholds[csCalButton] = (maxMe / 2) + (maxWaiting / 2) - CS_MIN_THRESHOLD;
				} else {
					// Otherwise, report the excursion distance from the steady-state "waiting" reading
					// that will recognize the weakest button press, with a slight margin to ensure it's read as a press.
					csThresholds[csCalButton] = minMe - CS_MIN_THRESHOLD;
				
					// If the weakest button press isn't distinguishable from noise or other buttons, note that
					// this button is just "OK" - you have to press it hard.
					if (maxOthers > minMe
						|| maxOthers > minMe - 2 * CS_MIN_THRESHOLD
						|| maxWaiting > minMe
						|| maxWaiting > minMe - CS_MIN_THRESHOLD)
					{
						csResults[csCalButton] = acrOK;
					} else
						csResults[csCalButton] = acrGreat;
				}
			} else {
				// Blank out the results for unused buttons, just for completeness.
				csResults[csCalButton] = acrFail;
				csThresholds[csCalButton] = 0;
			}
		}
	
		// Copy results back to EEPROM.
		write_eeprom_block(CAPSENSE_EEPROM_ADDR, (char*) csThresholds, CAPSENSE_EEPROM_LEN);
		
		// Copy intermediate results to the start of EEPROM, so they can be conveniently read out.
		// Overwrites whatever's there (ASCII table for the display at the moment).
		write_eeprom_block(0, (char*) csMaxWaiting, sizeof(csMaxWaiting));
		write_eeprom_block(sizeof(csMaxWaiting), (char*) csMaxOthers, sizeof(csMaxOthers));
		write_eeprom_block(sizeof(csMaxWaiting) + sizeof(csMaxOthers), (char*) csMaxHolding, sizeof(csMaxHolding));
	
		// Signal when done.
		return false;
	} else
		return true;
}

#endif
// CS_AUTO_CALIBRATE
