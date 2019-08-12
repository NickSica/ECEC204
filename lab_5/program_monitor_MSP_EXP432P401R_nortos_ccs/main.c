/* Description: Skeleton code for the program monitor
 *
 * Author: Naga Kandasamy
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* User-specific includes. */
#include "uart_functions.h"
#include "blur_filter.h"

void tic(void);
void toc(void);

/* Global variables containing the input and output images. */
image_t in, out;

uint16_t ticTime, tocTime;
int overflow;
const Timer_A_ContinuousModeConfig continuousModeConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,
    TIMER_A_CLOCKSOURCE_DIVIDER_1,
    TIMER_A_TAIE_INTERRUPT_ENABLE,
    TIMER_A_DO_CLEAR
};

void TA0_N_IRQHandler(void)
{
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    overflow++;
    return;
}

void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    int i;
    if (status & GPIO_PIN1)
    {
        SysTick_disableInterrupt();
        for(i = 0; i < 12000; i++);
        SysTick_enableInterrupt();
    }

    return;
}

void SysTick_Handler(void)
{
    WDT_A_clearTimer();
    return;
}

/* The tic function. */
void tic(void)
{
    ticTime = Timer_A_getCounterValue(TIMER_A0_BASE);
    overflow = 0;
    return;
}

/* The toc function. */
void toc(void)
{
    tocTime = Timer_A_getCounterValue(TIMER_A0_BASE);
    return;
}

int main(void)
{
    /* Setup Watchdog  */
    MAP_WDT_A_holdTimer();
    WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_SMCLK, WDT_A_CLOCKITERATIONS_8192K);
    SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);

    if (ResetCtl_getSoftResetSource() & RESET_SRC_1)
    {
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
        while (1)
        {
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            int i;
            for(i = 0; i < 80000; i++); // Delay
        }
    }

    /* Initialize the UART module. */
    initUART();
    writeString ("Established communication with the board");

    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz

    // Setup Timer16
    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);
    Interrupt_enableInterrupt(INT_TA0_N);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    SysTick_enableModule();
    SysTick_setPeriod(8191999);
    SysTick_enableInterrupt();

    // Setup up Button P1.1
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableMaster();

    WDT_A_startTimer();

    while(1)
    {
        tic();
        writeString ("\n\rAcquiring and processing image");
        acquireImage (&in, MIN_VALUE, MAX_VALUE, SIZE); // Acquire image
        blurFilter (&in, &out); // Process image
        toc();
        //int elapsedTime = (float)((0x10000 - ticTime) + (tocTime - 0) + (overflow - 1) * 0x10000) / (float)12000000;
        writeString ("\n\rDone");
        //writeInt(overflow);
        //writeString("\n");
        //writeFloat(elapsedTime);
    }
}
