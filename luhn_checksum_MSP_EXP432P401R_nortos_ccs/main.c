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
    initUART ();

    /* Write to UART. */
    writeString ("Established communication with the board");

    char identifier[BUFFER_SIZE];
    int checksum;
    uint8_t status;

    writeString ("\n\rEnter identification number: ");
    readString (identifier);

    writeString ("\n\rCalculating checksum for identifier ");
    writeString (identifier);

    /* FIXME: Place the checksum in argument 2 and its
     * validity in argument 3.
     */
    validateLuhn (identifier, &checksum, &status);
    if (status == 1)
        writeString ("\n\rIdentification number is valid.");
    else
        writeString ("\n\rIdentification number is invalid.");

    writeString ("\n\rIdling CPU.");
    while (1);
}

/* FIXME: Complete the function. The actual checksum should be
 * returned via argument 2 and the result of the check via argument 3.
 */
void
validateLuhn (char *identfier, int *checksum, uint8_t *status)
{
    *checksum = 0;
    *status = 0;

    /* FIXME: Write your code here. */

    return;
}
