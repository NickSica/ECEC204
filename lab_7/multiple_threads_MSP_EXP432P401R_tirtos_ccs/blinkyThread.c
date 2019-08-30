/* Decription: Thread that toggles LED1 at specified rate.
 */

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"

void *blinkyThread(void *arg0)
{
    mqd_t *mqdes = arg0;
    int delay;

    /* Call driver init functions. */
    GPIO_init ();

    /* Configure the LED pin and turn on LED1.  */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);

    while (1)
    {
        while (mq_receive(*mqdes, (int)&delay, sizeof(delay), NULL) != -1)
        {
            sleep(delay);
            GPIO_toggle(Board_GPIO_LED0);
        }

    }
}
