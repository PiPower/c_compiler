#include "DataEncoder.hpp"
#include <math.h>
#include <string.h>


static int64_t hexCharToDec(char c)
{
    if(c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if(c > 'F')
    {
        c -= 'f' - 'F';
    }

    return c - 'A' + 10;
}

static long double decStringToLongDouble(const char *data, int32_t len)
{
    long double x = 0;
    int64_t i = 0;

    while (data[i] >= '0' && data[i] <= '9' && i < len)
    {   
        long double v = data[i] - '0'; 
        x *= 10.0;
        i++;
        x+= v;
    }
    if(i < len && data[i] == '.')
    {
        i++;
        long double frac = 0.1;
        while (data[i] >= '0' && data[i] <= '9' && i < len)
        {   
            long double v = data[i] - '0'; 
            x += v * frac;
            i++;
            frac *= 0.1;
        }
    }

    return x;
}

static uint64_t decStringToUint64(const char *data, int32_t len)
{
    uint64_t x = 0;
    int32_t i = 0;
    
    while (i < len && data[i] >= '0' && data[i] <= '9')
    {   
        uint64_t v = data[i] - '0'; 
        x *= 10;
        i++;
        x+= v;
    }
    
    return x;
}

static uint64_t hexStringToUint64(const char *data, int32_t len)
{
    int64_t x = 0;
    int64_t i = 0;

    i++; //skip 0
    i++; // ski x
    int64_t v = hexCharToDec(data[i]);
    while (i < len && v >= 0 && v <= 15)
    {   
        x *= 16;
        i++;
        x+= v;
        v = hexCharToDec(data[i]); 
    }
    
    return x;
}

static int64_t decStringToInt64(const char *data, int32_t len)
{
    int64_t x = 0;
    int64_t i = 0;
    int64_t sign = 1;

    if(i < len && data[i] == '-')
    {
        sign = -1;
        i++;
    }

    while (i < len && data[i] >= '0' && data[i] <= '9')
    {   
        int64_t v = data[i] - '0'; 
        x *= 10;
        i++;
        x+= v;
    }
    
    return sign * x;
}

static int64_t hexStringToInt64(const char *data, int32_t len)
{
    int64_t x = 0;
    int64_t i = 0;
    int64_t sign = 1;

    if(i < len && data[i] == '-')
    {
        sign = -1;
        i++;
    }

    i++; //skip 0
    i++; // ski x
    int64_t v = hexCharToDec(data[i]);
    while (i < len && v >= 0 && v <= 15)
    {   
        x *= 16;
        i++;
        x+= v;
        v = hexCharToDec(data[i]); 
    }
    
    return sign * x;
}


long double stringToLongDouble(const char *data, int32_t len, uint8_t mode)
{
    if(mode == MODE_DEC)
    {
        return decStringToLongDouble(data, len);
    }
    
    return 0.0l;
}

double stringToDouble(const char *data, int32_t len, uint8_t mode)
{
    return (double)stringToLongDouble(data, len, mode);
}

int64_t stringToInt64(const char *data, int32_t len, uint8_t mode)
{
    if(mode == MODE_DEC)
    {
        return decStringToInt64(data, len);
    }
    else if(mode == MODE_HEX)
    {   
        return hexStringToInt64(data, len);
    }
    return 0;
}

uint64_t stringToUint64(const char *data, int32_t len, uint8_t mode)
{
    if(mode == MODE_DEC)
    {
        return decStringToUint64(data, len);
    }
    else if(mode == MODE_HEX)
    {   
        return hexStringToUint64(data, len);
    }
    return 0;
}

int64_t stringToInt64(const std::string_view &view, uint8_t mode)
{
    return stringToInt64(view.data(), view.length(), mode);
}

int64_t stringToChar(const char *data, int32_t len)
{
    int64_t dataOut = 0;
    memcpy(&dataOut, (void*)(data + 1), 1);
    
    return dataOut;
}
