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
        d_l_double,
        d_dynamic
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
        long double lFloat;
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
            case DType::d_l_double: return lhs.lFloat != rhs.lFloat;
            default:
                return true;
        }

        return true; // fallback (shouldn't happen)
    }
    template<typename T>
    inline T CastTo(const Number& num)
    {
        switch (num.type)
        {
            case DType::d_int8_t:   return (T)num.int8;;
            case DType::d_int16_t:  return (T)num.int16;
            case DType::d_int32_t:  return (T)num.int32;
            case DType::d_int64_t:  return (T)num.int64;

            case DType::d_uint8_t:  return (T)num.uint8;
            case DType::d_uint16_t: return (T)num.uint16;
            case DType::d_uint32_t: return (T)num.uint32;
            case DType::d_uint64_t: return (T)num.uint64;

            case DType::d_float:  return (T)num.float32;
            case DType::d_double: return (T)num.float64;
            case DType::d_l_double: return (T)num.lFloat;
            default: return 0;
        }

    }

    inline std::string ToString(const Number& num)
    {
        switch (num.type)
        {
            case DType::d_int8_t:   return std::to_string(num.int8);
            case DType::d_int16_t:  return std::to_string(num.int16);
            case DType::d_int32_t:  return std::to_string(num.int32);
            case DType::d_int64_t:  return std::to_string(num.int64);

            case DType::d_uint8_t:  return std::to_string(num.uint8);
            case DType::d_uint16_t: return std::to_string(num.uint16);
            case DType::d_uint32_t: return std::to_string(num.uint32);
            case DType::d_uint64_t: return std::to_string(num.uint64);

            case DType::d_float:  return std::to_string(num.float32);
            case DType::d_double: return std::to_string(num.float64);
            case DType::d_l_double: return std::to_string(num.lFloat);
            default: return "";
        }

    }
    template<template<typename> class OP>
    Typed::Number TypedBinOp(const Number& l, const Number& r)
    {
        Typed::Number out = {};
        if(l.type != r.type)
        {
            return out;
        }
        out.type = l.type;
        switch (out.type)
        {
            case DType::d_int8_t:   out.int8   = OP<uint8_t>{}(l.int8,   r.int8);   break;
            case DType::d_int16_t:  out.int16  = OP<uint16_t>{}(l.int16,  r.int16);  break;
            case DType::d_int32_t:  out.int32  = OP<uint32_t>{}(l.int32,  r.int32);  break;
            case DType::d_int64_t:  out.int64  = OP<uint64_t>{}(l.int64,  r.int64);  break;

            case DType::d_uint8_t:  out.uint8  = OP<int8_t>{}(l.uint8,  r.uint8);  break;
            case DType::d_uint16_t: out.uint16 = OP<int16_t>{}(l.uint16, r.uint16); break;
            case DType::d_uint32_t: out.uint32 = OP<int32_t>{}(l.uint32, r.uint32); break;
            case DType::d_uint64_t: out.uint64 = OP<int64_t>{}(l.uint64, r.uint64); break;

            case DType::d_float:    out.float32 = OP<float>{}(l.float32, r.float32); break;
            case DType::d_double:   out.float64 = OP<double>{}(l.float64, r.float64); break;
            case DType::d_l_double: out.lFloat  = OP<long double>{}(l.lFloat,  r.lFloat);  break;
            default: break;
        }

        return out;
    }

}


