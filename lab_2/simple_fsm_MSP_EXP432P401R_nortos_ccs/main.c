/* Description: Code for finite state machine (FSM) whose behavior is
 * dictated by BUTTON1 and BUTTON2.
 *
 * Author: Naga Kandasamy
 * Date created: July 5, 2019
 *******************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "debounce.h"

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    unsigned int btn1_state = SWITCH_RELEASED;
    unsigned int btn2_state = SWITCH_RELEASED;
    uint8_t btn1 = 0;
    uint8_t btn2 = 0;

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    uint32_t freq = CS_getDCOFrequency() / 300;
    int counter = freq;

    while (1)
    {
        btn1_state = getDebouncedSwitchState(btn1_state, &counter, 1);
        btn2_state = getDebouncedSwitchState(btn2_state, &counter, 2);
        if(btn1_state == SWITCH_PRESSED)
        {
            while(getDebouncedSwitchState(btn1_state, &counter, 1) != SWITCH_RELEASED);
            btn1 = btn1 ^ 1;
        }

        if(btn2_state == SWITCH_PRESSED)
        {
            while(getDebouncedSwitchState(btn2_state, &counter, 2) != SWITCH_RELEASED);
            btn2 = btn2 ^ 1;
        }

        if(btn1 == 0 && btn2 == 0)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            counter = freq;
        }
        else if(btn1 == 0 && btn2 == 1)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            if(counter <= 0)
            {
                GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
                counter = freq;
            }
            else
                counter--;
        }
        else if(btn1 == 1 && btn2 == 0)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            if(counter <= 0)
            {
                GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
                counter = freq;
            }
            else
                counter--;
        }
        else if(btn1 == 1 && btn2 == 1)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
            counter = freq;
        }
    }
}
