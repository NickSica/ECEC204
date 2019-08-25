/* Description: Skeleton code to measure fan speed using tachometer signals.
 *
 * Author: Naga Kandasamy
 * Date: August 14, 2019
*******************************************************************************/
/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"

/* Timer configuration: input capture when operating in continuous mode. */
const Timer_A_ContinuousModeConfig continuousModeConfig =
{
 TIMER_A_CLOCKSOURCE_ACLK,         // FIXME: Choose appropriate clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_64,     // 1:1 divider ratio for the counter clock
 TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable overflow ISR
 TIMER_A_DO_CLEAR
};

const Timer_A_ContinuousModeConfig continuousModeConfig2 =
{
 TIMER_A_CLOCKSOURCE_ACLK,         // FIXME: Choose appropriate clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_64,     // 1:1 divider ratio for the counter clock
 TIMER_A_TAIE_INTERRUPT_ENABLE,     // Disable overflow ISR
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

uint8_t isChanged = 1;
unsigned int numCaptures = 0;
uint16_t t0 = 0;
uint16_t n = 0xffff;
float rpm = 0;

void TA0_N_IRQHandler(void)
{
    /* First Option */ /*
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    uint16_t t1 = Timer_A_getCaptureCompareCount(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    uint16_t elapsedCount = t1 - t0;
    if(elapsedCount <= 0)
        elapsedCount = elapsedCount + n;

    float elapsedTime = (1 / 9400) * elapsedCount;
    rpm = 1 / (2 * elapsedTime) * 60;
    t0 = t1;
    */

    /* Third Option */
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    writeString("boop");
    float newRPM = (1 / (numCaptures / 2)) * 240;
    if(newRPM != rpm)
    {
        rpm = newRPM;
        isChanged = 1;
    }
    numCaptures = 0;

    return;

}

void TA1_N_IRQHandler(void)
{
    /* Third Option */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    numCaptures += 1;
    return;
}

int main(void)
{
    /* Stop watchdog.  */
    MAP_WDT_A_holdTimer();

    /* Initialize the high-speed clock system. */
     CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // DCO = 12 MHz
     CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
     CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
     CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
     CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ);
     CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_128);

     /* Derive ACLK from VLO oscillator, which operates at 9.4KHz. */
     //CS_initClockSignal (CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);

     /* Initialize the UART module. */
      initUART();
      writeString ("Established communication with the board");

      /* P2.4 is the primary module input to the capture Timer A0. (See pin schematic.) */
      GPIO_setAsPeripheralModuleFunctionInputPin (GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

      /* Configure timer(s) in continuous mode with input capture. */
      Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig2);
      Timer_A_configureContinuousMode(TIMER_A1_BASE, &continuousModeConfig);
      Timer_A_initCapture(TIMER_A1_BASE, &captureModeConfig);

      /* Enable interrupt processing for the timer(s). */
      Timer_A_enableCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
      Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
      Interrupt_enableInterrupt(INT_TA0_N);
      Interrupt_enableInterrupt(INT_TA1_N);
      Interrupt_enableMaster();

      /* Start timer(s). */
      Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
      Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);

      while (1)
      {
          if(isChanged == 1)
          {
              writeString("\r\nThe RPM is: ");
              writeFloat(rpm);
              isChanged = 0;
          }
      }
}
