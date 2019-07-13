/* Functions to perform string comparisons. */

#include <stdint.h>
#include "string_operations.h"


uint8_t compareStrings (char *str1, char *str2)
{
    char *ptrStr1 = str1;
    char *ptrStr2 = str2;
    while(*ptrStr1 != '\0' || *ptrStr2 != '\0')
    {
        if(*ptrStr1 != *ptrStr2)
            if(*ptrStr1 > *ptrStr2)
                return *ptrStr1 - *ptrStr2;
            else
                return *ptrStr2 - *ptrStr1;

        if(*ptrStr1 == '\0')
            return (uint8_t) *ptrStr2;
        else if(*ptrStr2 == '\0')
                    return (uint8_t) *ptrStr1;

        ptrStr1++;
        ptrStr2++;
    }

    return 0;
}

int8_t searchForChar (char *str, char ch)
{
    char *ptrStr = str;
    int loc = 0;

    while(*ptrStr != '\0')
    {
        if(*ptrStr == ch)
            return loc;

        ptrStr++;
        loc++;
    }


    return -1;
}

uint8_t searchSubString (char *str1, char *str2)
{
    char *ptrStr1 = str1;
    char *ptrStr2 = str2;
    int loc = 0;
    int finalLoc = -1;

    while(*ptrStr1 != '\0')
    {
        if(*ptrStr1 == *ptrStr2)
        {
            if(finalLoc == -1)
                finalLoc = loc;

            if(*ptrStr2 == '\0')
                return finalLoc;

            ptrStr2++;
        }

        ptrStr1++;
        loc++;
    }

    return 0;
}





