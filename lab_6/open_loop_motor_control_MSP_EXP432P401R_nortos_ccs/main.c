/* Description: Skeleton code for open-loop motor control.
 *
 * Author: Naga Kandasamy
 * Date created: August 13, 2019
*******************************************************************************/

/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"

/* PWM-specific defines. */
#define PWM_PERIOD 32000 // Timer period
#define DUTY_CYCLE_LENGTH 0.1 * PWM_PERIOD // Initial duty cycle length

/* Configuration parameters for Timer_A module in PWM configuration */
Timer_A_PWMConfig pwmConfig =
{
 TIMER_A_CLOCKSOURCE_ACLK,                  // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_1,             // Divider ratio, 1:1
 PWM_PERIOD,                                // Timer period which will stored in CCR0
 TIMER_A_CAPTURECOMPARE_REGISTER_1,         // Compare register to use
 TIMER_A_OUTPUTMODE_RESET_SET,              // Output mode for PWM signal
 DUTY_CYCLE_LENGTH                          // Duty cycle length which will be stored in CCR1
};

unsigned int getRPM (void);
void controlMotor (int);

int
main (void)
{
    /* Stop watchdog.  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the high-speed clock system. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Set reference oscillator to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART ();
    writeString ("Established communication with the board");


    /* Configure P2.4 as the pin that outputs the PWM signal. */
    GPIO_setAsPeripheralModuleFunctionOutputPin (GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configure PWM. */
    Timer_A_generatePWM (TIMER_A0_BASE, &pwmConfig);

    /* Configure and enable interrupt system. */
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Interrupt_enableInterrupt (INT_TA0_0);
    Interrupt_enableInterrupt (INT_TA0_N);
    Interrupt_enableMaster ();

    int desiredRPM = 0;
    while (1) {
        
        /* FIXME: Read desired RPM via the terminal into variable desiredRPM. */
        desiredRPM = getRPM();

        /* FIXME: Complete the function that provides the correct PWM signal to the motor. */
        controlMotor(desiredRPM);
    }
}

/* ISR to handle end of PWM period. */
void
TA0_0_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    /* User-defined code. */
    return;
}

/* ISR to handle end of PWM pulse. */
void
TA0_N_IRQHandler (void)
{
    Timer_A_clearCaptureCompareInterrupt (TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    /* User-defined code. */
    return;
}

/* Function gets desired RPM from the user. */
unsigned int getRPM(void)
{
    /* FIXME: Complete the function to return the desired RPM. */
    writeString("\r\nEnter the desired RPM: ");
    int input;
    readInt(&input);
    return input;
}

/* Function supplies PWM signal to achieve the desired RPM. */
void controlMotor(int desiredRPM)
{
    pwmConfig.dutyCycle = desiredRPM;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    return;
}
