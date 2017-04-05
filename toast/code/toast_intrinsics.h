#if !defined(TOAST_INTRINSICS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

#include "math.h"

inline void
SwapS32(s32 *A, s32 *B)
{
    s32 Temp = *A;
    *A = *B;
    *B = Temp;
}

inline void
SwapR32(r32 *A, r32 *B)
{
    r32 Temp = *A;
    *A = *B;
    *B = Temp;
}

inline r32
Square(r32 A)
{
    r32 Result = A*A;
    return(Result);
}

inline r32
Pow(r32 A, r32 B)
{
    r32 Result = powf(A, B);
    return(Result);
}

inline r32
Log(r32 R32)
{
    r32 Result = logf(R32);
    return(Result);
}

inline r32
FloorR32ToR32(r32 R32)
{
    r32 Result = floorf(R32);
    return(Result);
}

inline s32
FloorR32ToS32(r32 R32)
{
    s32 Result = (s32)FloorR32ToR32(R32);
    return(Result);
}

inline r32
CeilR32ToR32(r32 R32)
{
    r32 Result = ceilf(R32);
    return(Result);
}

inline s32
CeilR32ToS32(r32 R32)
{
    s32 Result = (s32)CeilR32ToR32(R32);
    return(Result);
}

inline r32
AbsoluteValue(r32 R32)
{
    r32 Result = (r32)fabs(R32);
    return(Result);
}

inline r32
Sin(r32 Angle)
{
    r32 Result = sinf(Angle);
    return(Result);
}

inline r32
Cos(r32 Angle)
{
    r32 Result = cosf(Angle);
    return(Result);
}

inline r32
ATan2(r32 Y, r32 X)
{
    r32 Result = atan2f(Y, X);
    return(Result);
}

inline r32
SquareRoot(r32 R32)
{
    r32 Result = sqrtf(R32);
    return(Result);
}

#define TOAST_INTRINSICS_H
#endif
