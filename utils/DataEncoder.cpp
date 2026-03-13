#include "DataEncoder.hpp"
#include <math.h>
#include <string.h>
static double decStringToDouble(const char *data, int32_t len)
{
    return 0.0;
}

static int64_t decStringToInt64(const char *data, int32_t len)
{
    int64_t x = 0;
    int64_t i = 0;

    while (data[i] >= '0' && data[i] <= '9' && i < len)
    {   
        // '0' - '9' < 128 so it makes no diffrence whether char == int8 or char == uint8
        int64_t v = data[i] - '0'; 
        x *= 10;
        i++;
        x+= v;
    }
    
    return x;
}

double stringToDouble(const char *data, int32_t len, uint8_t mode)
{
    if(mode == MODE_DEC)
    {
        return decStringToDouble(data, len);
    }
    
    return 0.0;
}

int64_t stringToInt64(const char *data, int32_t len, uint8_t mode)
{
    if(mode == MODE_DEC)
    {
        return decStringToInt64(data, len);
    }
    return 0;
}

int64_t stringToChar(const char *data, int32_t len)
{
    int64_t dataOut = 0;
    memcpy(&dataOut, (void*)(data + 1), 1);
    
    return dataOut;
}
