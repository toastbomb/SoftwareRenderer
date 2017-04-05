#if !defined(TOAST_MATH_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

inline r32
SafeRatioN(r32 Numerator, r32 Divisor, r32 N)
{
    r32 Result = N;

    if(Divisor != 0.0f)
    {
        Result = Numerator / Divisor;
    }

    return(Result);
}

inline r32
SafeRatio0(r32 Numerator, r32 Divisor)
{
    r32 Result = SafeRatioN(Numerator, Divisor, 0.0f);

    return(Result);
}

inline r32
SafeRatio1(r32 Numerator, r32 Divisor)
{
    r32 Result = SafeRatioN(Numerator, Divisor, 1.0f);

    return(Result);
}

union v2
{
    struct
    {
        r32 x, y;
    };

    struct
    {
        r32 u, v;
    };

    r32 E[3];
};

inline v2
V2(r32 X, r32 Y, r32 Z)
{
    v2 Result;
    Result.x = X;
    Result.y = Y;

    return(Result);
}

inline v2
operator*(r32 A, v2 B)
{
    v2 Result;

    Result.x = A*B.x;
    Result.y = A*B.y;

    return(Result);
}

inline v2
operator*(v2 A, r32 B)
{
    v2 Result = B*A;

    return(Result);
}

inline v2
operator/(v2 A, r32 B)
{
    v2 Result;

    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);

    return(Result);
}

inline v2
operator*=(v2 &A, r32 B)
{
    A = B*A;

    return(A);
}

inline v2
operator-(v2 A)
{
    v2 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return(Result);
}

inline v2
operator+=(v2 &A, v2 B)
{
    A = A + B;

    return(A);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return(Result);
}

inline v2
operator-=(v2 &A, v2 B)
{
    A = A - B;

    return(A);
}

union v3
{
    struct
    {
        r32 x, y, z;
    };
    struct
    {
        r32 r, g, b;
    };

    r32 E[3];
};

inline v3
V3(r32 X, r32 Y, r32 Z)
{
    v3 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

inline v3
operator*(r32 A, v3 B)
{
    v3 Result;

    Result.x = A*B.x;
    Result.y = A*B.y;
    Result.z = A*B.z;

    return(Result);
}

inline v3
operator*(v3 A, r32 B)
{
    v3 Result = B*A;

    return(Result);
}

inline v3
operator/(v3 A, r32 B)
{
    v3 Result;

    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);
    Result.z = SafeRatio0(A.z, B);

    return(Result);
}

inline v3
operator*=(v3 &A, r32 B)
{
    A = B*A;

    return(A);
}

inline v3
operator-(v3 A)
{
    v3 Result;

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;    

    return(Result);
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;    

    return(Result);
}

inline v3
operator+=(v3 &A, v3 B)
{
    A = A + B;

    return(A);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;    

    return(Result);
}

inline v3
operator-=(v3 &A, v3 B)
{
    A = A - B;

    return(A);
}

inline v3
Lerp(v3 A, r32 T, v3 B)
{
    v3 Result = (1.0f - T)*A + T*B;

    return(Result);
}

inline r32
Inner(v3 A, v3 B)
{
    r32 Result = A.x*B.x + A.y*B.y + A.z*B.z;

    return(Result);
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result = {(A.y*B.z - B.y*A.z), (-A.x*B.z + B.x*A.z), (A.x*B.y - B.x*A.y)};
    
    return(Result);
}

inline r32
LengthSq(v3 A)
{
    r32 Result = Inner(A, A);

    return(Result);
}

inline r32
Length(v3 A)
{
    r32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v3
Normalize(v3 A)
{
    v3 Result = A / Length(A);

    return(Result);
}
    
union v4
{
    struct
    {
        r32 x, y, z, w;
    };
    struct
    {
        r32 r, g, b, a;
    };

    r32 E[3];
};

inline v4
V4(r32 X, r32 Y, r32 Z, r32 W)
{
    v4 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;

    return(Result);
}

inline v4
operator*(r32 A, v4 B)
{
    v4 Result;

    Result.x = A*B.x;
    Result.y = A*B.y;
    Result.z = A*B.z;
    Result.w = A*B.w;

    return(Result);
}

inline v4
operator*(v4 A, r32 B)
{
    v4 Result = B*A;

    return(Result);
}

inline v4
operator/(v4 A, r32 B)
{
    v4 Result;

    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);
    Result.z = SafeRatio0(A.z, B);
    Result.w = SafeRatio0(A.w, B);

    return(Result);
}

inline v4
operator*=(v4 &A, r32 B)
{
    A = B*A;

    return(A);
}

inline v4
operator-(v4 A)
{
    v4 Result;

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;    
    Result.w = -A.w;    

    return(Result);
}

inline v4
operator+(v4 A, v4 B)
{
    v4 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;    
    Result.w = A.w + B.w;    

    return(Result);
}

inline v4
operator+=(v4 &A, v4 B)
{
    A = A + B;

    return(A);
}

inline v4
operator-(v4 A, v4 B)
{
    v4 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;    
    Result.w = A.w - B.w;    

    return(Result);
}

inline v4
operator-=(v4 &A, v4 B)
{
    A = A - B;

    return(A);
}

inline v4
Lerp(v4 A, r32 T, v4 B)
{
    v4 Result = (1.0f - T)*A + T*B;

    return(Result);
}

inline r32
Inner(v4 A, v4 B)
{
    r32 Result = A.x*B.x + A.y*B.y + A.z*B.z + A.w*B.w;

    return(Result);
}

inline r32
LengthSq(v4 A)
{
    r32 Result = Inner(A, A);

    return(Result);
}

inline r32
Length(v4 A)
{
    r32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline v4
Normalize(v4 A)
{
    v4 Result = A / Length(A);

    return(Result);
}

struct vertex
{
    union
    {
        struct
        {
            v3 P;
        };
        struct
        {
            r32 x, y, z;
        };
    };

    union
    {
        struct
        {
            v4 Color;
        };
        struct
        {
            r32 r, g, b, a;
        };
    };

    union
    {
        struct
        {
            v2 UV;
        };
        struct
        {
            r32 u, v;
        };
    };

    union
    {
        struct
        {
            v3 N;
        };
        struct
        {
            r32 Nx, Ny, Nz;
        };
    };

    r32 Li;
};

inline vertex
operator-(vertex A)
{
    vertex Result;

    Result.P = -A.P;
    Result.Color = -A.Color;
    Result.UV = -A.UV;
    Result.Li = -A.Li;
    
    return(Result);
}

inline vertex
operator-(vertex A, vertex B)
{
    vertex Result;

    Result.P = A.P - B.P;
    Result.Color = A.Color - B.Color;
    Result.UV = A.UV - B.UV;
    Result.Li = A.Li - B.Li;
    
    return(Result);
}

inline vertex
operator+(vertex A, vertex B)
{
    vertex Result;

    Result.P = A.P + B.P;
    Result.Color = A.Color + B.Color;
    Result.UV = A.UV + B.UV;
    Result.Li = A.Li + B.Li;
    
    return(Result);
}

inline vertex
operator*(r32 A, vertex B)
{
    vertex Result;

    Result.P = A*B.P;
    Result.Color = A*B.Color;
    Result.UV = A*B.UV;
    Result.Li = A*B.Li;
    
    return(Result);
}

inline vertex
operator*(vertex A, r32 B)
{
    vertex Result = B*A;

    return(Result);
}

inline vertex
operator/(vertex A, r32 B)
{
    vertex Result;

    Result.P = A.P / B;
    Result.Color = A.Color / B;
    Result.UV = A.UV / B;
    Result.Li = A.Li / B;
    
    return(Result);
}

struct quaternion
{
    v3 xyz;
    r32 w;
};

inline quaternion
Quaternion(v3 A)
{
    quaternion Result;
    
    Result.xyz = A;
    Result.w = 1.0f;

    return(Result);
}

inline quaternion
operator-(quaternion A)
{
    quaternion Result;

    Result.xyz = -A.xyz;
    Result.w = -A.w;
    
    return(Result);
}

inline quaternion
operator+(quaternion A, quaternion B)
{
    quaternion Result;

    Result.xyz = A.xyz + B.xyz;
    Result.w = A.w + B.w;

    return(Result);
}

inline quaternion
operator-(quaternion A, quaternion B)
{
    quaternion Result;

    Result.xyz = A.xyz - B.xyz;
    Result.w = A.w - B.w;

    return(Result);
}

inline r32
Inner(quaternion A, quaternion B)
{
    r32 Result = Inner(A.xyz, B.xyz) + (A.w * B.w);

    return(Result);
}

inline quaternion
operator*(quaternion A, quaternion B)
{
    quaternion Result;

    Result.xyz = A.w * B.xyz + B.w * A.xyz + Cross(A.xyz, B.xyz);
    Result.w = A.w * B.w - Inner(A.xyz, B.xyz);

    return(Result);
}

inline quaternion
operator*(quaternion A, r32 B)
{
    quaternion Result;

    Result.xyz = A.xyz * B;
    Result.w = A.w * B;

    return(Result);
}

inline quaternion
operator*(r32 A, quaternion B)
{
    quaternion Result;

    Result = B*A;
    
    return(Result);
}

inline quaternion
operator/(quaternion A, r32 B)
{
    quaternion Result;

    Result.xyz = A.xyz / B;
    Result.w = A.w / B;

    return(Result);
}

inline quaternion
operator/(r32 A, quaternion B)
{
    quaternion Result;

    Result = B / A;
    
    return(Result);
}
    
inline r32
LengthSq(quaternion A)
{
    r32 Result = Inner(A, A);

    return(Result);
}

inline r32
Length(quaternion A)
{
    r32 Result = SquareRoot(LengthSq(A));

    return(Result);
}

inline quaternion
Normalize(quaternion A)
{
    quaternion Result = A*(1.0f / Length(A));

    return(Result);
}

inline quaternion
Conjugate(quaternion A)
{
    quaternion Result;

    Result.xyz = -A.xyz;
    Result.w = A.w;

    return(Result);
}

inline quaternion
Inverse(quaternion A)
{
    quaternion Result = Conjugate(A) / LengthSq(A);

    return(Result);
}

inline v3
operator*(quaternion A, v3 B)
{
    v3 Result;

    v3 t = 2.0f*Cross(A.xyz, B);
    Result = (B + (A.w*t) + Cross(A.xyz, t));

    return(Result);
}

inline quaternion
RotationQuaterion(v3 Axis, r32 Radian)
{
    quaternion Result;

    Axis = Normalize(Axis);
    Result.xyz = Axis * Sin(Radian*0.5f);
    Result.w = Cos(Radian*0.5f);
    
    return(Result);
}

inline v3
RotateV3(quaternion Axis, v3 V)
{
    quaternion Result;
    quaternion QuatV = {V, 0};

    quaternion AxisConjugate = Conjugate(Axis);
    Result = AxisConjugate*QuatV*Axis;

    return(Result.xyz);
}

#define TOAST_MATH_H
#endif
