/* Description: Adjust the blinking rate of the LED dynamically based on
 * input from BUTTON1.
 *
 * Author: Naga Kandasamy
 * Date created: July 5, 2019
*******************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* User Includes */
#include "debounce.h"

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    unsigned int btnState = SWITCH_RELEASED;

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    uint32_t freq = CS_getDCOFrequency() / 100;
    uint8_t multiplier = 5;

    int maxCount = freq*multiplier;
    int counter = 0;

    while (1)
    {
        while(counter < maxCount)
        {
            btnState = getDebouncedSwitchState(btnState, &counter, &maxCount);
            if(btnState == SWITCH_PRESSED)
            {
                while(getDebouncedSwitchState(btnState, &counter, &maxCount) != SWITCH_RELEASED)
                {
                    counter++;
                    if(counter == maxCount)
                    {
                        GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);
                        counter = 0;
                    }
                }

                btnState = SWITCH_RELEASED;
                if(multiplier == 1)
                {
                    multiplier = 5;
                }
                else
                {
                    multiplier--;
                }
                maxCount = freq*multiplier;
            }
            counter++;
        }

        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        counter = 0;
    }
}
