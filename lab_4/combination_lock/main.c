/* Description: Skeleton code for lab assignment.
 * Calculate elapsed time between pressing switches S1 and switch S2. Account for timer overflows.
 * Display elapsed time on the terminal.
 *
 * Author: Giri Prabhu
 * Modified by: Naga Kandasamy, July 29, 2019
*******************************************************************************/

/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* User-defined includes. */
#include "uart_functions.h"

/* Configuration for timer in continuous mode. */
const Timer_A_ContinuousModeConfig continuousModeConfig = {
 TIMER_A_CLOCKSOURCE_ACLK, // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_1, // Divider ratio
 TIMER_A_TAIE_INTERRUPT_ENABLE, // Enable TAIE interrupt
 TIMER_A_DO_CLEAR
};

typedef enum
{
    UNLOCKED, LOCKED
} states;

int getCombination()
{
    int currState = -1;
    char input;
    while(1)
    {
        input = readString();
        switch(input)
        {
        case '1':
            if(currState == 4)
                currState = 1;
            else
                return 0;
            break;
        case '2':
            if(currState == -1)
                currState = 2;
            else
                return 0;
            break;
        case '3':
            if(currState == 5)
                return 1;
            else
                return 0;
            break;
        case '4':
            if(currState == 2)
                currState = 5;
            else
                return 0;
            break;
        case '5':
            if(currState == 1)
                currState = 5;
            else
                return 0;
            break;
        default:
            return 0;
        }
    }
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the clock system. Do not change the below configuration. */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12); // DCO = 12 MHz
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART();
    writeString ("Established communication with the board");

    /* Configure BUTTON1 (P1.1) and BUTTON2 (P1.4). */
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

    /* Configure and start Timer_A0. Configure the interrupt processing system. */

    states state = UNLOCKED;
    while(1)
    {
        switch(state)
        {
        case UNLOCKED:
            char *input = readString();
            if(strcmp(input, '1'))
            break;
        case LOCKED:

            break;
        }
    }

}

void
TA0_N_IRQHandler (void)
{
    /* FIXME: Write ISR code to clear the TAIE interrupt and count number of overflows. */
    return;
}

