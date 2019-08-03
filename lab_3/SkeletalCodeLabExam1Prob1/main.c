/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>


uint8_t colorcode;

const eUSCI_UART_Config uartConfig=
{
 EUSCI_A_UART_CLOCKSOURCE_SMCLK,
 78,//BRDIV
 2, //UCxBRF
 0,//UCxBRS
 EUSCI_A_UART_NO_PARITY,
 EUSCI_A_UART_LSB_FIRST,
 EUSCI_A_UART_ONE_STOP_BIT,
 EUSCI_A_UART_MODE,
 EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

//Interrupt handler receives and echoes character typed in serial terminal
void EUSCIA0_IRQHandler(void)
    {
        uint32_t status=UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
        UART_clearInterruptFlag(EUSCI_A0_BASE,status);
        if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
        {
            colorcode = UART_receiveData(EUSCI_A0_BASE);
            UART_transmitData(EUSCI_A0_BASE, colorcode);
        }

    }

void halfsecdelay()
{
    uint32_t i;
    for (i=0;i<100000;i++);
    return;
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    // P1.2 and P1.3 are UART TXD and RXD. These pins must be put in special function mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2|GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
    //Sets DCO frequency at 12 MHz to get correct baud rate of 9600
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableMaster();

    /* Fill in code here to initialize LED2*/
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    uint8_t prevColor;

    while(1)
    {
        switch(colorcode)
        {
        case 'R':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
            prevColor = 'R';
            break;
        case 'r':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
            prevColor = 'r';
            halfsecdelay();
            break;
        case 'G':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
            prevColor = 'G';
            break;
        case 'g':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2);
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
            prevColor = 'g';
            halfsecdelay();
            break;
        case 'B':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
            prevColor = 'B';
            break;
        case 'b':
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
            halfsecdelay();
            prevColor = 'b';
            break;
        default:
            switch(prevColor)
            {
            case 'r':
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
                GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
                prevColor = 'r';
                halfsecdelay();
                break;
            case 'g':
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2);
                GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
                prevColor = 'g';
                halfsecdelay();
                break;
            case 'b':
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
                GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
                halfsecdelay();
                prevColor = 'b';
                break;
            }
            break;
        }

    }
}
