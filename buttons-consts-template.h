// buttons-consts.h
// Customize this to fit your application.


#ifndef __BUTTONS_CONSTS
#define __BUTTONS_CONSTS

// Define this if buttons are pulled up on press, rather than pulled down (the default).
//#define BUTTONS_ACTIVE_HIGH

// The port used for buttons - all buttons must be on the same port.
#define BUTTON_PORT  portb
#define BUTTON_TRIS  trisb

// The button constants, sequential.
// Replace these with the buttons needed for your app.
// These are the pin numbers of each button on the port, as well as 
// symbolic constants returned by GetButton().
#define PREV_BTN  0
#define NEXT_BTN  1

// The individual button masks.
#define PREV_BTN_MASK  (1 << PREV_BTN)
#define NEXT_BTN_MASK  (1 << NEXT_BTN)
#define ALL_BTNS_MASK  (PREV_BTN_MASK | NEXT_BTN_MASK)
// If there's only one button, do this like: (ONE_BTN_MASK | 0) due to a BoostC macro expansion bug.

#define FIRST_BTN  PREV_BTN
#define LAST_BTN  NEXT_BTN

// Minimum down events before recognizing a button press, in button check periods (often, ms).
#define MIN_DOWNS  40  // max 254


#endif