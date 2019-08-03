#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

#define DELAY 1000
#define SWITCH_PRESSED 2
#define SWITCH_RELEASED 3

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Function prototype definitions. */
unsigned int getSwitchState(int btn);
unsigned int getDebouncedSwitchState (unsigned int previousState, int *counter, int btn);

#endif
