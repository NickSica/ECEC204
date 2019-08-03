/* DriverLib includes. */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes. */
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

/* User Includes. */
#include "uart_functions.h"

int main(void)
{
    MAP_WDT_A_holdTimer();

    /* Configure the output GPIO pins driving the LEDs. */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    /* Configure input GPIO pins, P1.1 for BUTTON1 and BUTTON2 P1.4. */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4,
    GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableMaster();

    initUART();
    writeString("\n\rEstablished communication with the board");
    Interrupt_disableInterrupt(INT_EUSCIA0);

    while (1)
    {
        char color[BUFFER_SIZE];
        if(Interrupt_isEnabled(INT_EUSCIA0))
        {
            writeString("\n\rEnter the first letter of the color: ");
            readString(color);
            color[0] = toupper(color[0]);
            writeString("\n\rChanging Color");
        }

        switch (color[0])
        {
        case 'O':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
            break;

        case 'R':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
            break;

        case 'G':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2);
            break;

        case 'B':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
            break;

        case 'Y':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;

        case 'P':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            break;

        case 'C':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;

        case 'W':
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
            break;
        }
    }

    return 0;
}

/* ISR to handle IRQs generated by pins in PORT1. */
void PORT1_IRQHandler(void)
{
    uint16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    /* Check for the correct pin that generated the interrupt. */
    if (status & GPIO_PIN1)
    {
        Interrupt_enableInterrupt(INT_EUSCIA0);
    }
    else if (status & GPIO_PIN4)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
        Interrupt_disableInterrupt(INT_EUSCIA0);
    }

    return;
}