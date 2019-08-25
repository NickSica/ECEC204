/* Description: Skeleton code for wall clock program.
 *
 * Author: Naga Kandasamy
 * Date: August 7, 2019
*******************************************************************************/

/* DriverLib includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* User-specific includes. */
#include "uart_functions.h"

#define TIMER_PERIOD_0 32000

const Timer_A_UpModeConfig upModeConfig =
{
     TIMER_A_CLOCKSOURCE_ACLK,
     TIMER_A_CLOCKSOURCE_DIVIDER_1,
     TIMER_PERIOD_0,
     TIMER_A_TAIE_INTERRUPT_ENABLE,
     TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
     TIMER_A_DO_CLEAR
};

uint8_t timeChange = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

void getTime()
{
    char *rawStartingTime = "00 00 00";
    writeString("Please input time(hh mm ss):");
    readString(rawStartingTime);
    if(strlen(rawStartingTime) > 8)
    {
        writeString("Invalid time, using previous time.");
    }
    else
    {
        hours = atoi(strtok(rawStartingTime, " "));
        minutes = atoi(strtok(NULL, " "));
        seconds = atoi(strtok(NULL, "\n"));
    }
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer ();

    /* Initialize the clock system. */
    CS_setDCOCenteredFrequency (CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency (CS_REFO_32KHZ); // Reference oscillator is set to 32KHz
    CS_initClockSignal (CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART();
    writeString("Established communication with the board");

    /* Initialize variables associated with the wall clock. */
    getTime();

    /* Initialize Timer A module and set up the interrupt processing system. */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    //Timer_A_enableInterrupt(TIMER_A0_BASE);
    Interrupt_enableInterrupt(INT_TA0_N);
    Interrupt_enableMaster();

    while(1)
    {
        /* Update variables associated with the wall-clock based on user input
         * from the UART.
         */
        uint8_t input = 'h';
        readChar(input);
        if(input == 'h')
        {
            if(timeChange)
            {
                char outputTime[10];
                sprintf(outputTime, "%d:%d:%d", hours, minutes, seconds);
            /*
            if(seconds % 100 < 10)
                strSec = strcat("0", itoa(seconds));
            else
                strSec = itoa(seconds);

            if(minutes % 100 < 10)
                strMin = strcat("0", itoa(minutes));
            else
                strMin = itoa(minutes);

            if(hours % 100 < 10)
                strHr = strcat("0", itoa(hours));
            else
                strHr = itoa(hours);

            char *outputTime = strcat(strcat(strcat(strcat(strHr, ":"), strMin), ":"), strSec);
            */
                writeString(outputTime);
                timeChange = 0;
            }
        }
        else if(input == 'c')
        {
            Timer_A_stopTimer(TIMER_A0_BASE);
            Timer_A_clearTimer(TIMER_A0_BASE);
            getTime();
            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
        }
    }
}

void TA0_N_IRQHandler(void)
{
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    seconds++;
    if(seconds == 60)
    {
        minutes++;
        seconds -= 60;
    }

    if(minutes == 60)
    {
        hours++;
        seconds -= 60;
    }

    if(hours == 60)
    {
        hours = 0;
        seconds = 0;
        minutes = 0;
    }
    timeChange = 1;
}
