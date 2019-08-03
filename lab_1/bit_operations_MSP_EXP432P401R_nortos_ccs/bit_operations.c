/* Functions that implement bit-level operations
 *
 */

#include <stdint.h>
#include "bit_operations.h"

uint8_t countBits(uint8_t v)
{
    uint8_t count = 0;
    int i;

    for(i = 0; i <= 7; i++)
    {
        if(((v << i) & 0b10000000) == 0b10000000)
            count++;
    }

    return count;
}

int isPowerOfTwo(uint8_t v)
{
    int status = 0;
    int i;

    for(i = 0; i <= 7; i++)
    {
        if(((v << i) & 0b10000000) == 0b10000000)
            if(status == 1)
            {
                status = 0;
                break;
            }
            else
                status = 1;
    }

    return status;
}

uint8_t rearrangeBits(uint8_t v)
{
    uint8_t rv = 0b00000000;
    int pos = 0;
    int i;

    for(i = 0; i <= 7; i++)
    {
        if(((v << i) & 0b10000000) == 0b10000000)
        {
            rv = rv | (0b10000000 >> pos);
            pos++;
        }
    }

    return rv;
}
