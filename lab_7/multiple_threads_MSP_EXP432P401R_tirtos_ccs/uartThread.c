/* Description: Thread to interact with the terminal via the UART.
 */

#include <stdint.h>
#include <stddef.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "Board.h"

void *uartThread(void *arg0)
{
    char        input;
    const char  echoPrompt[] = "Echoing characters:\r\n";
    UART_Handle uart;
    UART_Params uartParams;
    mqd_t *mqdes = arg0;

    /* Call driver init functions */
    UART_init();

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL)
    {
        /* UART_open() failed */
        while (1);
    }

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    /* Loop forever echoing input received from the UART. */
    while (1)
    {
        UART_read(uart, &input, 1);

        if(input <= '1' || input >= '6')
            UART_write(uart, "Incorrect input.", sizeof("Incorrect input."));
        else
            mq_send(*mqdes, (char *)&input, sizeof(msg), 0);

    }
}



