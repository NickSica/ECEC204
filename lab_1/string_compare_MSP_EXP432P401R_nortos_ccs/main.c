/* Description: Program that implements simple operations that
 * perform string comparisons.
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
#include "uart_functions.h"
#include "string_operations.h"

int main(void)
{
    /* Halt watchdog timer.  */
    MAP_WDT_A_holdTimer();
    /* Initialize the UART module. */
    initUART();
    writeString("Established communication with the board");

    int choice;
    uint8_t status, ch;
    char str1[BUFFER_SIZE], str2[BUFFER_SIZE];

    while(1)
    {
        writeString("\n \r Function to execute (1 for string compare, 2 for character search, 3 for sub-string search, or -1 to exit");
        readInt(&choice);

        if(choice == -1)
            break;

        switch(choice)
        {
        case 1:
            readString(&str1);
            readString(&str2);
            status = compareStrings (str1, str2);
            break;

        case 2:
            readString(&str1);
            readString(&ch);
            status = searchForChar (str1, ch);
            writeInt (status);
            break;

        case 3:
            readString(&str1);
            readString(&str2);
            status = searchSubString (str1, str2);
            writeInt (status);
            break;

        default:
            writeString ("\n \r Erroneous entry. Try again");
        }
    }

    writeString("\n \r All done. Idling CPU.");

    while(1);
}
