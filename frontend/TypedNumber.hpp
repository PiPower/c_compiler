#pragma once
#include <inttypes.h>

namespace Typed
{
    enum DType : uint8_t
    {
        d_none = 0, 

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

    inline bool operator!=(const Number& lhs, const Number& rhs)
    {
        if (lhs.type != rhs.type)
        {
            return true;
        }

        switch (lhs.type)
        {
            case DType::d_none: return false; 
            
            case DType::d_int8_t:   return lhs.int8   != rhs.int8;
            case DType::d_int16_t:  return lhs.int16  != rhs.int16;
            case DType::d_int32_t:  return lhs.int32  != rhs.int32;
            case DType::d_int64_t:  return lhs.int64  != rhs.int64;

            case DType::d_uint8_t:  return lhs.uint8  != rhs.uint8;
            case DType::d_uint16_t: return lhs.uint16 != rhs.uint16;
            case DType::d_uint32_t: return lhs.uint32 != rhs.uint32;
            case DType::d_uint64_t: return lhs.uint64 != rhs.uint64;

            case DType::d_float:  return lhs.float32 != rhs.float32;
            case DType::d_double: return lhs.float64 != rhs.float64;

            default:
                return true;
        }

        return true; // fallback (shouldn't happen)
    }
}


