#include "debounce.h"

/* Get the debounced state of the switch. */
unsigned int getDebouncedSwitchState(unsigned int previousState, int *counter, int *maxCount)
{
    unsigned int currentState = getSwitchState(); /* Get current state of the switch. */
    if (currentState == previousState) /* State is unchanged. */
        return previousState;

    /* Instantaneous state has changed. Wait for it to stabilize using
     * debouncing algorithm. The state has to remain unchanged for four
     * consecutive sampling periods. */
    unsigned int i = 0, j = 0;
    unsigned int nextState;

    while (j != 0x001E)
    {
        nextState = getSwitchState();
        if (currentState == nextState)
        {
            j |= 0x0001;
            j = j << 1;
        }
        else
            j = 0;

        currentState = nextState;

        /* Delay. Needs to be tuned by programmer for the debounce
         * algorithm to work correctly. Usually switch specific. */
        for (i = DELAY; i > 0; i--)
        {
            (*counter)++;
            if(*counter == *maxCount)
            {
                *counter = 0;
                GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);
            }
        }
        (*counter)++;
        if(*counter == *maxCount)
        {
            *counter = 0;
            GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);
        }
    }

    return currentState;
}

/* Returns instantaneous state of the switch. */
unsigned int getSwitchState(void)
{
    if (GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN1) == GPIO_INPUT_PIN_LOW)
        return SWITCH_PRESSED;
    else
        return SWITCH_RELEASED;
}
