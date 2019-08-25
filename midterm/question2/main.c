/* Description: Skeleton code for the program monitor
 *
 * Author: Naga Kandasamy
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

void delay()
{
    long int i;
    for(i = 0; i < 256000; i++);
}

int main(void)
{
    /* Setup Watchdog  */
    MAP_WDT_A_holdTimer();

    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK, WDT_A_CLOCKITERATIONS_32K);
    SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    Interrupt_enableMaster();

    WDT_A_startTimer();

    delay();
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    while(1);

}
