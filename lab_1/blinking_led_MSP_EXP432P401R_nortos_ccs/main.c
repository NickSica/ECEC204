#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "uart_functions.h"

void printHello()
{
    writeString("Hello World");
}

void print100()
{
    int i;
    for(i = 1; i <= 100; i++)
    {
        writeInt(i);
    }
}

void printFP()
{
    float i;
    for(i = 1; i <= 10; i += 0.1)
    {
        i = roundf(i * 10) / 10;
        writeFloat(i);
    }
}

int main (void)
{
    uint32_t i, j = 1;
    uint32_t loopBound = 1200000; /* Bound set to default value. */
    uint8_t ch;

    WDT_A_hold(WDT_A_BASE); /* Stop watchdog timer */
    initUART(); /* Initialize UART. */
    writeString("Established communication with the board");

    //Practice methods
    //printHello();
    //print100();
    //printFP();

    /* Set P1.0, to output direction */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Set DCO to 12 MHz. */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* Main loop that runs on the board */
    while(1)
    {
        /* Peek into the ring buffer to see if we have a byte of data
        waiting to be read. If so return 1, else return 0. */
        if(peek())
        {
            /* Non-blocking function. */
            ch = readChar(); /* Read a character from ring buffer. */
            j = ch - '0'; /* Convert ASCII to integer. */
            if(j <= 0)
                j = 1;
            else if(j >= 6)
                j = 5;
        }

        /* Delay for some time using nops on the processor */
        for(i = loopBound * j; i > 0; i --);

        /* Toggle Pin. */
        GPIO_toggleOutputOnPin (GPIO_PORT_P1, GPIO_PIN0);
    }
}
