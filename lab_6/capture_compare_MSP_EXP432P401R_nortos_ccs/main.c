/* Description: Calculate the frequency of ACLK which is output on P4.2
 * Output from P4.2 is provided as input to the the timer module on P2.4
 * via a jumper cable.
 *
 * Modified from code listing 7.13 (page 143) in the textbook
 *
 * Author: Giri Prabhu
 * Modified: Naga Kandasamy, August 12, 2019
*******************************************************************************/

/* DriverLib includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"

#define NUM_VALUES 25

/* Timer configuration: input capture when operating in continuous mode. */
const Timer_A_ContinuousModeConfig continuousModeConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,         // Clock source is SMCLK
 TIMER_A_CLOCKSOURCE_DIVIDER_1,     // 1:1 divider ratio for the counter clock
 TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable overflow ISR
 TIMER_A_DO_CLEAR
};

const Timer_A_CaptureModeConfig captureModeConfig =
{
 TIMER_A_CAPTURECOMPARE_REGISTER_1,     // Register 1 holds the capture value
 TIMER_A_CAPTUREMODE_RISING_EDGE,       // Capture on rising edge
 TIMER_A_CAPTURE_INPUTSELECT_CCIxB,     // Input select
 TIMER_A_CAPTURE_SYNCHRONOUS,
 TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE, // Enable capture interrupt
 TIMER_A_OUTPUTMODE_OUTBITVALUE
};

/* Array holding the frequency estimates. */
volatile float captureFreq[NUM_VALUES];
uint8_t done = 0;

 int
 main (void)
 {
     MAP_WDT_A_holdTimer (); // Hold watchdog timer

     /* Initialize the high-speed clock system. */
     CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // DCO = 12 MHz
     CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
     CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
     CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

     /* Derive ACLK from VLO oscillator, which operates at 9.4KHz. */
     CS_initClockSignal (CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);

     /* Initialize the UART module. */
      initUART ();
      writeString ("Established communication with the board");

     /* P4.2 is the primary module output for ACLK. (See pin schematic.) */
     GPIO_setAsPeripheralModuleFunctionOutputPin (GPIO_PORT_P4, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
     /* P2.4 is the primary module input Timer A0. (See pin schematic.) */
     GPIO_setAsPeripheralModuleFunctionInputPin (GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

     /* Configure timer in continuous mode with input capture. */
     Timer_A_configureContinuousMode (TIMER_A0_BASE, &continuousModeConfig);
     Timer_A_initCapture (TIMER_A0_BASE, &captureModeConfig);

     /* Enable interrupt processing for the timer. */
     Interrupt_enableInterrupt (INT_TA0_N);
     Interrupt_enableMaster ();

     /* Start timer. */
     Timer_A_startCounter (TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

     /* Let CPU idle. */
     while (1);
}

 /* ISR to handle capture events. */
 void
 TA0_N_IRQHandler (void)
 {
     static uint8_t i;
     static uint16_t curr = 0;
     static uint16_t prev = 0;
     uint8_t j;
     float sum;

     Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);

     /* Time stamp the event that occurred on P2.4. */
     if (i < NUM_VALUES) {
         curr = Timer_A_getCaptureCompareCount (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
         captureFreq[i++] = (float)12000000/(curr - prev);
         prev = curr;
     }
     else if (done == 0) { /* Calculate the frequency. */
         sum = 0.0;
         for (j = 0; j < NUM_VALUES; j++)
             sum += captureFreq[j];
         writeString ("\n\rEstimated ACLK frequency: ");
         writeFloat (sum/NUM_VALUES);
         writeString (" Hz");
         done = 1;
     }

     return;
 }
