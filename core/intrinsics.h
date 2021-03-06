#ifndef TC_INTRINSICS_H
#define TC_INTRINSICS_H

#include "types.h"
#include <math.h>

#define PI (3.14159265359f)

inline f32 f32_infinity()
{
    f32 result = INFINITY;
    return result;
}

inline f32 f32_abs(f32 f)
{
    return fabsf(f);
}

inline f32 f32_min_2(f32 f0, f32 f1)
{
    return f0 < f1 ? f0 : f1;
}

inline float f32_min_3(f32 f0, f32 f1, f32 f2)
{
    return f32_min_2(f32_min_2(f0, f1), f2);
}

inline float f32_max_2(f32 f0, f32 f1)
{
    return f0 > f1 ? f0 : f1;
}

inline float f32_max_3(f32 f0, f32 f1, f32 f2)
{
    return f32_max_2(f32_max_2(f0, f1), f2);
}

inline f32 f32_rad(f32 a)
{
    f32 result = (a / 180.0f) * PI;
    return result;
}

inline f32 f32_sqrt(f32 v)
{
    return sqrtf(v);
}

 inline f32 f32_sin(f32 a)
 {
    return sinf(a);
 }

 inline f32 f32_cos(f32 a)
 {
    return cosf(a);
 }

 inline f32 f32_tan(f32 a)
 {
    return tanf(a);
 }

#endif // TC_INTRINSICS_H
