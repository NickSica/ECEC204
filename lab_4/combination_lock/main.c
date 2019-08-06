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
#include <string.h>

/* User-defined includes. */
#include "uart_functions.h"

#define TIMER_PERIOD 60000000

/* Configuration for timer in continuous mode. */
const Timer_A_ContinuousModeConfig continuousModeConfig = {
 TIMER_A_CLOCKSOURCE_ACLK, // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_1, // Divider ratio
 TIMER_A_TAIE_INTERRUPT_ENABLE, // Enable TAIE interrupt
 TIMER_A_DO_CLEAR
};

uint8_t codeTimeout;
char *code = "24153";

typedef enum
{
    UNLOCKED, LOCKED, NEW_CODE
} states;

int getCombination()
{
    int currPos = 0;
    char *input;
    writeString("Please enter the code to unlock the system: ");
    Timer32_startTimer(TIMER32_0_BASE, 0);
    while(1)
    {
        readChar(input);
        if(currPos == 0)
            Timer32_setCount(TIMER32_0_BASE, TIMER_PERIOD);

        if(codeTimeout == 1)
        {
            writeString("Code input took too long, please try again: ");
            currPos = 0;
            codeTimeout = 0;
            continue;
        }

        if(strcmp(&(code[currPos]), input))
            currPos++;
        else
            return 0;

        if(currPos == 5)
            return 1;
    }
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Set SMCLK to 12MHz for UART comms.
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableInterrupt(INT_TA0_N);
    Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);
    Interrupt_enableMaster();

    /* Initialize the UART module. */
    initUART();
    writeString ("Established communication with the board");

    /* Configure BUTTON1 (P1.1) and BUTTON2 (P1.4). */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);

    states state = UNLOCKED;
    uint8_t attempts = 0;
    while(1)
    {
        switch(state)
        {
        case UNLOCKED:
            writeString("Device Unlocked: ");
            Timer_A_stopTimer(TIMER_A0_BASE);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            char *input;
            readChar(input);
            if(*input == '1')
                state = LOCKED;
            else if(*input == '2')
                state = NEW_CODE;
            break;
        case LOCKED:
            if(attempts <= 2)
            {
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
                uint8_t isCorrect = getCombination();
                Timer32_haltTimer(TIMER32_0_BASE);
                if(isCorrect == 1)
                {
                    state = UNLOCKED;
                    attempts = 0;
                }
                else
                    attempts++;
            }
            break;
        case NEW_CODE:
            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
            writeString("Enter New Key: ");
            char newCode1[5];
            int i;
            for(i = 0; i < 5; i++)
                readChar(&(newCode1[i]));

            writeString("Enter Key Again to Confirm: ");
            char newCode2[5];
            for(i = 0; i < 5; i++)
                readChar(&(newCode2[i]));

            if(strcmp(newCode1, newCode2) == 0)
            {
                *code = *newCode1;
                state = UNLOCKED;
            }
            else
            {
                writeString("Codes do not match, keeping old code.");
                state = UNLOCKED;
            }
        }
    }

}

void TA0_N_IRQHandler(void)
{
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
    return;
}

void T32_INT1_IRQHandler(void)
{
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    codeTimeout = 1;
}


