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
#include <math.h>
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

int overflow = 0;
uint8_t btn1 = 0;
uint8_t btn2 = 0;

void
delay (uint8_t msecs)
{
    uint32_t i;
    for (i = 0;i < 275 * msecs; i++);
    return;
}

void TA0_N_IRQHandler(void)
{
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    overflow++;
    return;
}

void PORT1_IRQHandler(void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if(status & GPIO_PIN1)
        btn1 = 1;

    if(status & GPIO_PIN4)
        btn2 |= 2;
}

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the clock system. Do not change the below configuration. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // DCO = 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART();
    writeString ("Established communication with the board");

    /* Configure BUTTON1 (P1.1) and BUTTON2 (P1.4). */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

    /* Configure and start Timer_A0. Configure the interrupt processing system. */
    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    Interrupt_enableInterrupt(INT_TA0_N);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableMaster();
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    /* Capture and calculate the elapsed time between button presses. */
    uint16_t start = 0;
    uint16_t end = 0;
    while(1)
    {
        if(btn1)
        {
            start = Timer_A_getCounterValue(TIMER_A0_BASE);
            btn1 = 0;
            btn2 = 1;
            overflow = 0;
        }

        if(btn2 == 3)
        {
            end = Timer_A_getCounterValue(TIMER_A0_BASE);
            int elapsed = round((float)((0x10000 - start) + (end - 0) + (overflow - 1) * 0x10000) / (float)32000);
            writeInt(elapsed);
            btn2 = 0;
        }
    }
}


