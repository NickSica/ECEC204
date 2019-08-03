/* Description: A luhn checksum validator.
 *
 * Author: Naga Kandasamy
 * Date created: June 28, 2019
 * Date modified:
*******************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/* User includes */
#include "uart_functions.h"

/* Function prototype */
void validateLuhn (char *identifier, int *checksum, uint8_t *status);

int
main (void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Initialize the UART module. */
    initUART();

    /* Write to UART. */
    writeString("Established communication with the board");

    char identifier[BUFFER_SIZE];
    int checksum;
    uint8_t status;

    writeString("\n\rEnter identification number: ");
    readString(identifier);

    writeString("\n\rCalculating checksum for identifier ");
    writeString(identifier);

    /* FIXME: Place the checksum in argument 2 and its
     * validity in argument 3.
     */
    validateLuhn(identifier, &checksum, &status);
    if (status == 1)
        writeString("\n\rIdentification number is valid.");
    else
        writeString("\n\rIdentification number is invalid.");

    writeString("\n\rIdling CPU.");
    while (1);
}

/* FIXME: Complete the function. The actual checksum should be
 * returned via argument 2 and the result of the check via argument 3.
 */
void validateLuhn(char *identifier, int *checksum, uint8_t *status)
{
    *checksum = 0;
    *status = 0;
    int len = strlen(identifier);
    char *identifierPtr = (identifier+len) - 1;
    char *end = identifierPtr;

    while(identifierPtr != (identifier-1))
    {
        if((end - identifierPtr) % 2 == 1)
        {
            int doubleNum = (int)(*identifierPtr -  '0') * 2;
            *checksum += (doubleNum % 10) + ((doubleNum / 10) % 10);
        }
        else
        {
            *checksum += (int)(*identifierPtr - '0');
        }
        identifierPtr--;
    }
    *status = (*checksum % 10 == 0);

    return;
}
