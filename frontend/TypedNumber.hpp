#pragma once
#include <inttypes.h>

namespace Typed
{
    enum DType : uint8_t
    {
        d_int8_t,
        d_int16_t,
        d_int32_t,
        d_int64_t,

        d_uint8_t ,
        d_uint16_t,
        d_uint32_t,
        d_uint64_t,

        d_float,      
        d_double,   
    };

    struct Number
    {
        union {
        // Signed
        int8_t   int8;
        int16_t  int16;
        int32_t  int32;
        int64_t  int64;

        // Unsigned
        uint8_t  uint8;
        uint16_t uint16;
        uint32_t uint32;
        uint64_t uint64;

        // floats
        float       float32;
        double      float64;
        };

        Typed::DType type;
    };
}


