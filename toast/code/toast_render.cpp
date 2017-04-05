/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

inline v4
ConvertToLinearLight(v4 Color)
{
    v4 Result;

    r32 OneOver255 = 1.0f / 255.0f;

    Result.r = Color.r*Color.r*OneOver255*OneOver255;
    Result.g = Color.g*Color.g*OneOver255*OneOver255;
    Result.b = Color.b*Color.b*OneOver255*OneOver255;
    
    Result.a = Color.a*OneOver255;

    return(Result);
}

inline v4
ConvertToRGB(v4 Color)
{
    v4 Result;

    Result.r = SquareRoot(Color.r)*255.0f;
    Result.g = SquareRoot(Color.g)*255.0f;
    Result.b = SquareRoot(Color.b)*255.0f;
    
    Result.a = Color.a*255.0f;

    return(Result);
}

inline vertex
ConvertToPerspectiveLinear(vertex Vertex)
{
    vertex Result;

    Result.x = Vertex.x;
    Result.y = Vertex.y;

    Result.z = 1.0f / Vertex.z;
    Result.u = Vertex.u / Vertex.z;
    Result.v = Vertex.v / Vertex.z;

    Result.Color = Vertex.Color / Vertex.z;
    Result.Li = Vertex.Li / Vertex.z;
    
    return(Result);
}

inline vertex
ConvertToPerspective(vertex Vertex)
{
    vertex Result;

    Result.x = Vertex.x;
    Result.y = Vertex.y;

    Result.z = 1.0f / Vertex.z;
    Result.u = Vertex.u / Vertex.z;
    Result.v = Vertex.v / Vertex.z;
    
    Result.Color = Vertex.Color / Vertex.z;
    Result.Li = Vertex.Li / Vertex.z;
    
    return(Result);
}

internal void
DrawPixel(game_back_buffer *Buffer,
          s32 X, s32 Y,
          r32 Red, r32 Green, r32 Blue)
{
    if(X >= 0 && X <= Buffer->Width-1 && Y >= 0 && Y <= Buffer->Height-1)
    {
        u32 *Pixel = (u32 *)Buffer->Memory + (Y*Buffer->Width + X);

        u08 IntRed = (u08)(Red*255);
        u08 IntGreen = (u08)(Green*255);
        u08 IntBlue = (u08)(Blue*255);
    
        *Pixel = (IntRed << 16 | IntGreen << 8 | IntBlue);
    }
}

inline void
DrawPixelU32(game_back_buffer *Buffer,
             s32 X, s32 Y,
             u32 Color)
{
    u32 *Pixel = (u32 *)Buffer->Memory + (Y*Buffer->Width + X);

    *Pixel = Color;
}

inline v4
BilinearTextureSample(texture *Texture, r32 U, r32 V)
{
    v4 Result = {};

    U = U*(r32)(Texture->Width - 2);
    V = V*(r32)(Texture->Height - 2);

    s32 X = FloorR32ToS32(U);
    s32 Y = FloorR32ToS32(V);

    r32 UDiff = U - X;
    r32 VDiff = V - Y;
    r32 UDiffInv = 1 - UDiff;
    r32 VDiffInv = 1 - VDiff;

    u32 *TexelA = (u32 *)Texture->Memory + (Y*Texture->Width + X);
    u32 *TexelB = (u32 *)Texture->Memory + (Y*Texture->Width + (X + 1));

    u32 *TexelC = (u32 *)Texture->Memory + ((Y*Texture->Width + 1) + X);
    u32 *TexelD = (u32 *)Texture->Memory + ((Y*Texture->Width + 1) + (X + 1));

    v4 TexelAV4 = {(r32)((*TexelA >> 16) & 0xFF),
                   (r32)((*TexelA >> 8) & 0xFF),
                   (r32)((*TexelA) & 0xFF),
                   (r32)((*TexelA >> 24) & 0xFF)};

    v4 TexelBV4 = {(r32)((*TexelB >> 16) & 0xFF),
                   (r32)((*TexelB >> 8) & 0xFF),
                   (r32)((*TexelB) & 0xFF),
                   (r32)((*TexelB >> 24) & 0xFF)};

    v4 TexelCV4 = {(r32)((*TexelC >> 16) & 0xFF),
                   (r32)((*TexelC >> 8) & 0xFF),
                   (r32)((*TexelC) & 0xFF),
                   (r32)((*TexelC >> 24) & 0xFF)};

    v4 TexelDV4 = {(r32)((*TexelD >> 16) & 0xFF),
                   (r32)((*TexelD >> 8) & 0xFF),
                   (r32)((*TexelD) & 0xFF),
                   (r32)((*TexelD >> 24) & 0xFF)};

    Result = ((TexelAV4 * UDiffInv + TexelBV4 * UDiff) * VDiffInv +
              (TexelCV4 * UDiffInv + TexelDV4 * UDiff) * VDiff);
    
    return(Result);
}

inline u32
SampleTexture(game_back_buffer *Buffer, texture *Texture, s32 X, s32 Y, r32 R32U, r32 R32V, v4 Tint, r32 Li)
{
    u32 Result = 0;

    if(R32U > 1.0f)
    {
        R32U = 1.0f;
    }

    if(R32V > 1.0f)
    {
        R32V = 1.0f;
    }

    if(R32U < 0.0f)
    {
        R32U = 0.0f;
    }

    if(R32V < 0.0f)
    {
        R32V = 0.0f;
    }
    
    s32 TU = (s32)(R32U*(Texture->Width - 2) + 0.5f);
    s32 TV = (s32)(R32V*(Texture->Height - 2) + 0.5f);

    if(X >= 0 && X <= Buffer->Width-1 && Y >= 0 && Y <= Buffer->Height-1)
    {
        Assert(TU >= 0 && TU <= Texture->Width-1 && TV >= 0 && TV <= Texture->Height-1);

        u32 *Pixel = (u32 *)Buffer->Memory + (Y*Buffer->Width + X);
        u32 *Texel = (u32 *)Texture->Memory + (TV*Texture->Width + TU);

        Tint.r *= Tint.a;
        Tint.g *= Tint.a;
        Tint.b *= Tint.a;

#if 1
        v4 TexelV4 = {(r32)((*Texel >> 16) & 0xFF),
                      (r32)((*Texel >> 8) & 0xFF),
                      (r32)((*Texel) & 0xFF),
                      (r32)((*Texel >> 24) & 0xFF)};
#else
        v4 TexelV4 = BilinearTextureSample(Texture, R32U, R32V);
#endif
        
        TexelV4 = ConvertToLinearLight(TexelV4);
        
        TexelV4.r *= (Tint.r);
        TexelV4.g *= (Tint.g);
        TexelV4.b *= (Tint.b);
        TexelV4.a *= Tint.a;
        
        TexelV4.r *= Li;
        TexelV4.g *= Li;
        TexelV4.b *= Li;
        
        r32 InvNormTexelA = (1.0f - TexelV4.a);

        v4 PixelV4 = {(r32)((*Pixel >> 16) & 0xFF),
                      (r32)((*Pixel >> 8) & 0xFF),
                      (r32)((*Pixel) & 0xFF),
                      (r32)((*Pixel >> 24) & 0xFF)};
        PixelV4 = ConvertToLinearLight(PixelV4);
            
        r32 NormPixelA = PixelV4.a;

        v4 FinalPixel = {InvNormTexelA*PixelV4.r + TexelV4.r,
                         InvNormTexelA*PixelV4.g + TexelV4.g,
                         InvNormTexelA*PixelV4.b + TexelV4.b,
                         (NormPixelA + TexelV4.a -
                          NormPixelA*TexelV4.a)};
            
        FinalPixel = ConvertToRGB(FinalPixel);

#if 1
        if(FinalPixel.r > 255.0f)
        {
            FinalPixel.r = 255.0f;
        }

        if(FinalPixel.g > 255.0f)
        {
            FinalPixel.g = 255.0f;
        }

        if(FinalPixel.b > 255.0f)
        {
            FinalPixel.b = 255.0f;
        }
#endif
        Result = (((u32)(FinalPixel.a + 0.5f) << 24) |
                  ((u32)(FinalPixel.r + 0.5f) << 16) |
                  ((u32)(FinalPixel.g + 0.5f) << 8) |
                  ((u32)(FinalPixel.b + 0.5f)));
    }
    
    return(Result);
}

internal void
DrawTexture(game_back_buffer *Buffer, texture *Texture, r32 R32X, r32 R32Y, v4 Tint)
{
    s32 MinX = (s32)(R32X);
    s32 MinY = (s32)(R32Y);
    s32 MaxX = (s32)(R32X + (r32)Texture->Width);
    s32 MaxY = (s32)(R32Y + (r32)Texture->Height);

    if(MinX < 0)
    {
        MinX = 0;
    }

    if(MinY < 0)
    {
        MinY = 0;
    }

    if(MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }

    if(MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    u08 *PixelRow = ((u08 *)Buffer->Memory +
                      MinX*Buffer->BytesPerPixel +
                      MinY*Buffer->Pitch);
    u32 *TexelRow = (u32 *)Texture->Memory;

    Tint.r *= Tint.a;
    Tint.g *= Tint.a;
    Tint.b *= Tint.a;
    
    for(int Y = MinY;
        Y < MaxY;
        ++Y)
    {
        u32 *Pixel = (u32 *)PixelRow;
        u32 *Texel = (u32 *)TexelRow;
        for(int X = MinX;
            X < MaxX;
            ++X)
        {
            v4 TexelV4 = {(r32)((*Texel >> 16) & 0xFF),
                          (r32)((*Texel >> 8) & 0xFF),
                          (r32)((*Texel) & 0xFF),
                          (r32)((*Texel >> 24) & 0xFF)};
            TexelV4 = ConvertToLinearLight(TexelV4);
            
            TexelV4.r *= Tint.r;
            TexelV4.g *= Tint.g;
            TexelV4.b *= Tint.b;
            TexelV4.a *= Tint.a;
            
            r32 InvNormTexelA = (1.0f - TexelV4.a);

            v4 PixelV4 = {(r32)((*Pixel >> 16) & 0xFF),
                          (r32)((*Pixel >> 8) & 0xFF),
                          (r32)((*Pixel) & 0xFF),
                          (r32)((*Pixel >> 24) & 0xFF)};
            PixelV4 = ConvertToLinearLight(PixelV4);
            
            r32 NormPixelA = PixelV4.a;

            v4 FinalPixel = {InvNormTexelA*PixelV4.r + TexelV4.r,
                             InvNormTexelA*PixelV4.g + TexelV4.g,
                             InvNormTexelA*PixelV4.b + TexelV4.b,
                             (NormPixelA + TexelV4.a -
                              NormPixelA*TexelV4.a)};
            
            FinalPixel = ConvertToRGB(FinalPixel);

            *Pixel++ = (((u32)(FinalPixel.a + 0.5f) << 24) |
                        ((u32)(FinalPixel.r + 0.5f) << 16) |
                        ((u32)(FinalPixel.g + 0.5f) << 8) |
                        ((u32)(FinalPixel.b + 0.5f)));
            
            *Texel++;
        }

        PixelRow += Buffer->Pitch;
        TexelRow += Texture->Width;
    }
}

internal r32
MipMapSelection(triangle *Triangle, mip_map *MipMap)
{
    r32 Result = 0;

    if(Triangle)
    {
        r32 BaseTextureArea = (r32)MipMap->Textures[0].Width*(r32)MipMap->Textures[0].Height;
        r32 TriangleArea = (Length(Cross(Triangle->V[1].P - Triangle->V[0].P,
                                         Triangle->V[2].P - Triangle->V[0].P)) / 2);

        r32 MipRatio = BaseTextureArea / TriangleArea;
        Result = Log(MipRatio) / Log(4);
    }

    return(Result);
}

internal void
DrawFlatBottomTriangle(game_back_buffer *Buffer, triangle *Triangle, mip_map *MipMap,
                       game_back_buffer *ZBuffer, v4 Tint)
{
    r32 MipRatio = MipMapSelection(Triangle, MipMap);

    if(MipRatio < 0)
    {
        MipRatio = 0;
    }

    s32 MipIndex = FloorR32ToS32(MipRatio);
    texture *Texture = &MipMap->Textures[0];
    
    if(MipIndex <= 8)
    {
        Texture = &MipMap->Textures[MipIndex];
    }
        
    s32 TopIndex = 0;
    s32 LeftIndex = 1;
    s32 RightIndex = 2;
    
    if(Triangle->V[TopIndex].P.y < Triangle->V[LeftIndex].y)
    {
        SwapS32(&TopIndex, &LeftIndex);
    }
    
    if(Triangle->V[TopIndex].y < Triangle->V[RightIndex].y)
    {
        SwapS32(&TopIndex, &RightIndex);
    }
    
    if(Triangle->V[LeftIndex].x > Triangle->V[RightIndex].x)
    {
        SwapS32(&LeftIndex, &RightIndex);           
    }
    
    vertex VL = Triangle->V[LeftIndex];
    vertex VR = Triangle->V[RightIndex];
    vertex VT = Triangle->V[TopIndex];
    
    VL = ConvertToPerspectiveLinear(VL);
    VR = ConvertToPerspectiveLinear(VR);
    VT = ConvertToPerspectiveLinear(VT);
    
    vertex DvL = ((VL - VT) /
                  (VL.y - VT.y));

    vertex DvR = ((VR - VT) /
                  (VR.y - VT.y));
            
    
    r32 StartX = VL.x;
    r32 EndX = VR.x;
    
    r32 RowMin = CeilR32ToR32(VL.y);
    r32 RowMax = FloorR32ToR32(VT.y);

    r32 SubPixelAdjustment = RowMin - VL.y;

    StartX += DvL.x*SubPixelAdjustment;
    EndX += DvR.x*SubPixelAdjustment;

    VL.Color += DvL.Color*SubPixelAdjustment;
    VR.Color += DvR.Color*SubPixelAdjustment;

    VL.UV += DvL.UV*SubPixelAdjustment;
    VR.UV += DvR.UV*SubPixelAdjustment;

    VL.Li += DvL.Li*SubPixelAdjustment;
    VR.Li += DvR.Li*SubPixelAdjustment;

    for(s32 Y = (s32)RowMin;
        Y <= (s32)RowMax;
        ++Y)
    {
        s32 StartXCeil = CeilR32ToS32(StartX);
        s32 EndXFloor = FloorR32ToS32(EndX);

        vertex DvX = (VR - VL) / (EndX - StartX);
        vertex VX = VL;
        
        for(s32 X = StartXCeil;
            X <= EndXFloor;
            ++X)
        {
            vertex VP = ConvertToPerspective(VX);
#if 1
            u32 Color = SampleTexture(Buffer, Texture, X, Y, VP.u, VP.v, Tint, VP.Li);
#else
            s32 NextMipIndex = MipIndex;
            
            if(MipIndex < 8)
            {
                NextMipIndex++;
            }

            texture *NextTexture = &MipMap->Textures[NextMipIndex];
            u32 ColorA = SampleTexture(Buffer, Texture, X, Y, VP.u, VP.v, VP.Color);
            u32 ColorB = SampleTexture(Buffer, NextTexture, X, Y, VP.u, VP.v, VP.Color);
            
            v4 ColorAV4 = {(r32)((ColorA >> 16) & 0xFF),
                           (r32)((ColorA >> 8) & 0xFF),
                           (r32)((ColorA) & 0xFF),
                           (r32)((ColorA >> 24) & 0xFF)};
            
            v4 ColorBV4 = {(r32)((ColorB >> 16) & 0xFF),
                           (r32)((ColorB >> 8) & 0xFF),
                           (r32)((ColorB) & 0xFF),
                           (r32)((ColorB >> 24) & 0xFF)};

            v4 BlendedColor = Lerp(ColorAV4,
                                   (MipRatio - (r32)MipIndex),
                                   ColorBV4);
            u32 Color = (((u32)(BlendedColor.a + 0.5f) << 24) |
                         ((u32)(BlendedColor.r + 0.5f) << 16) |
                         ((u32)(BlendedColor.g + 0.5f) << 8) |
                         ((u32)(BlendedColor.b + 0.5f)));
            
#endif
            if(X >= 0 && X <= Buffer->Width-1 && Y >= 0 && Y <= Buffer->Height-1)
            {
                //rawPixelU32(Buffer, X, Y, Color);
                r32 *ZBufferDist = ((r32 *)ZBuffer->Memory + (Y * ZBuffer->Width) + X);
                if(VP.z > *ZBufferDist)
                {
                    *ZBufferDist = VP.z;
                    DrawPixelU32(Buffer, X, Y, Color);
                }
            }
            
            VX.Color += DvX.Color;  
            VX.UV += DvX.UV;
            VX.z += DvX.z;
            VX.Li += DvX.Li;
        }

        StartX += DvL.x;
        EndX += DvR.x;

        VL.Color += DvL.Color;
        VR.Color += DvR.Color;

        VL.UV += DvL.UV;
        VR.UV += DvR.UV;

        VL.z += DvL.z;
        VR.z += DvR.z;
        
        VL.Li += DvL.Li;
        VR.Li += DvR.Li;
    }
}

internal void
DrawFlatTopTriangle(game_back_buffer *Buffer, triangle *Triangle, mip_map *MipMap,
                    game_back_buffer *ZBuffer, v4 Tint)
{
    r32 MipRatio = MipMapSelection(Triangle, MipMap);
    
    if(MipRatio < 0)
    {
        MipRatio = 0;
    }

    s32 MipIndex = FloorR32ToS32(MipRatio);
    texture *Texture = &MipMap->Textures[0];
    
    if(MipIndex <= 8)
    {
        Texture = &MipMap->Textures[MipIndex];
    }
    
    s32 BottomIndex = 0;
    s32 LeftIndex = 1;
    s32 RightIndex = 2;
    
    if(Triangle->V[BottomIndex].y > Triangle->V[LeftIndex].y)
    {
        SwapS32(&BottomIndex, &LeftIndex);
    }
    
    if(Triangle->V[BottomIndex].y > Triangle->V[RightIndex].y)
    {
        SwapS32(&BottomIndex, &RightIndex);
    }
    
    if(Triangle->V[LeftIndex].x > Triangle->V[RightIndex].x)
    {
        SwapS32(&LeftIndex, &RightIndex);           
    }
    
    vertex VL = Triangle->V[LeftIndex];
    vertex VR = Triangle->V[RightIndex];
    vertex VB = Triangle->V[BottomIndex];
    
    VL = ConvertToPerspectiveLinear(VL);
    VR = ConvertToPerspectiveLinear(VR);
    VB = ConvertToPerspectiveLinear(VB);

    vertex DvL = ((VB - VL) /
                  (VB.y - VL.y));

    vertex DvR = ((VB - VR) /
                  (VB.y - VR.y));
    
    r32 StartX = VB.x;
    r32 EndX = VB.x;

    vertex VLs = VB;
    vertex VRs = VB;

    r32 RowMin = CeilR32ToR32(Triangle->V[BottomIndex].y);
    r32 RowMax = FloorR32ToR32(Triangle->V[LeftIndex].y);

    r32 SubPixelAdjustmentY = RowMin - Triangle->V[BottomIndex].y;

    StartX += DvL.x*SubPixelAdjustmentY;
    EndX += DvR.x*SubPixelAdjustmentY;

    VLs.Color += DvL.Color*SubPixelAdjustmentY;
    VRs.Color += DvR.Color*SubPixelAdjustmentY;

    VLs.UV += DvL.UV*SubPixelAdjustmentY;
    VRs.UV += DvR.UV*SubPixelAdjustmentY;

    VLs.Li += DvL.Li*SubPixelAdjustmentY;
    VRs.Li += DvR.Li*SubPixelAdjustmentY;

    for(s32 Y = (s32)RowMin;
        Y <= (s32)RowMax;
        ++Y)
    {
        s32 StartXCeil = CeilR32ToS32(StartX);
        s32 EndXFloor = FloorR32ToS32(EndX);

        vertex DvX = (VRs - VLs) / (EndX - StartX);
        vertex VX = VLs;

        for(s32 X = StartXCeil;
            X <= EndXFloor;
            ++X)
        {
            vertex VP = ConvertToPerspective(VX);
#if 1
            u32 Color = SampleTexture(Buffer, Texture, X, Y, VP.u, VP.v, Tint, VP.Li);
#else
            s32 NextMipIndex = MipIndex;
            
            if(MipIndex < 8)
            {
                NextMipIndex++;
            }

            texture *NextTexture = &MipMap->Textures[NextMipIndex];
            u32 ColorA = SampleTexture(Buffer, Texture, X, Y, VP.u, VP.v, VP.Color);
            u32 ColorB = SampleTexture(Buffer, NextTexture, X, Y, VP.u, VP.v, VP.Color);
            
            v4 ColorAV4 = {(r32)((ColorA >> 16) & 0xFF),
                           (r32)((ColorA >> 8) & 0xFF),
                           (r32)((ColorA) & 0xFF),
                           (r32)((ColorA >> 24) & 0xFF)};
            
            v4 ColorBV4 = {(r32)((ColorB >> 16) & 0xFF),
                           (r32)((ColorB >> 8) & 0xFF),
                           (r32)((ColorB) & 0xFF),
                           (r32)((ColorB >> 24) & 0xFF)};

            v4 BlendedColor = Lerp(ColorAV4,
                                   (MipRatio - (r32)MipIndex),
                                   ColorBV4);
            u32 Color = (((u32)(BlendedColor.a + 0.5f) << 24) |
                         ((u32)(BlendedColor.r + 0.5f) << 16) |
                         ((u32)(BlendedColor.g + 0.5f) << 8) |
                         ((u32)(BlendedColor.b + 0.5f)));
            
#endif
            if(X >= 0 && X <= Buffer->Width-1 && Y >= 0 && Y <= Buffer->Height-1)
            {
                r32 *ZBufferDist = ((r32 *)ZBuffer->Memory + (Y * ZBuffer->Width) + X);
                if(VP.z > *ZBufferDist)
                {
                    *ZBufferDist = VP.z;
                    DrawPixelU32(Buffer, X, Y, Color);
                }
            }
            
            VX.Color += DvX.Color;
            VX.UV += DvX.UV;
            VX.z += DvX.z;
            VX.Li += DvX.Li;
        }

        StartX += DvL.x;
        EndX += DvR.x;

        VLs.Color += DvL.Color;
        VRs.Color += DvR.Color;

        VLs.UV += DvL.UV;
        VRs.UV += DvR.UV;

        VLs.z += DvL.z;
        VRs.z += DvR.z;

        VLs.Li += DvL.Li;
        VRs.Li += DvR.Li;
    }
}

internal void
DrawTriangle(game_back_buffer *Buffer, triangle *Triangle, mip_map *MipMap,
             game_back_buffer *ZBuffer, v4 Tint)
{
    s32 TopIndex = 0;
    s32 MiddleIndex = 1;
    s32 BottomIndex = 2;
    
    if(Triangle->V[BottomIndex].y > Triangle->V[TopIndex].y)
    {
        SwapS32(&BottomIndex, &TopIndex);
    }
    
    if(Triangle->V[BottomIndex].y > Triangle->V[MiddleIndex].y)
    {
        SwapS32(&BottomIndex, &MiddleIndex);
    }
    
    if(Triangle->V[MiddleIndex].y > Triangle->V[TopIndex].y)
    {
        SwapS32(&MiddleIndex, &TopIndex);           
    }

    if(Triangle->V[MiddleIndex].y == Triangle->V[BottomIndex].y)
    {
        DrawFlatBottomTriangle(Buffer, Triangle, MipMap, ZBuffer, Tint);
    }
    else if(Triangle->V[MiddleIndex].y == Triangle->V[TopIndex].y)
    {
        DrawFlatTopTriangle(Buffer, Triangle, MipMap, ZBuffer, Tint);
    }
    else
    {
        vertex Split = Triangle->V[BottomIndex] -
            (Triangle->V[BottomIndex].y - Triangle->V[MiddleIndex].y) *
            ((Triangle->V[TopIndex] - Triangle->V[BottomIndex]) /
             (Triangle->V[TopIndex].y - Triangle->V[BottomIndex].y));
        
        triangle FlatBottomTriangle = {};
        FlatBottomTriangle.V[0] = Triangle->V[TopIndex];
        FlatBottomTriangle.V[1] = Triangle->V[MiddleIndex];
        FlatBottomTriangle.V[2].x = Split.x;
        FlatBottomTriangle.V[2].y = Triangle->V[MiddleIndex].y;
        FlatBottomTriangle.V[2].z = Triangle->V[MiddleIndex].z;
            
        FlatBottomTriangle.V[2].Color = Split.Color;
        FlatBottomTriangle.V[2].UV = Split.UV;
        FlatBottomTriangle.V[2].N = Split.N;
        FlatBottomTriangle.V[2].Li = Split.Li;

        DrawFlatBottomTriangle(Buffer, &FlatBottomTriangle, MipMap, ZBuffer, Tint);

        triangle FlatTopTriangle = {};
        FlatTopTriangle.V[0] = Triangle->V[BottomIndex];
        FlatTopTriangle.V[1] = Triangle->V[MiddleIndex];
        FlatTopTriangle.V[2].x = Split.x;
        FlatTopTriangle.V[2].y = Triangle->V[MiddleIndex].y;
        FlatTopTriangle.V[2].z = Triangle->V[MiddleIndex].z;

        FlatTopTriangle.V[2].Color = Split.Color;
        FlatTopTriangle.V[2].UV = Split.UV;
        FlatTopTriangle.V[2].N = Split.N;
        FlatTopTriangle.V[2].Li = Split.Li;

        DrawFlatTopTriangle(Buffer, &FlatTopTriangle, MipMap, ZBuffer, Tint);
    }    
}

internal void
ResetZBuffer(game_back_buffer *ZBuffer)
{
    s32 TopLeftY = 0;
    s32 TopLeftX = 0;
    s32 Width = ZBuffer->Width;
    s32 Height = ZBuffer->Height;
    
    for(s32 Y = TopLeftY;
        Y < TopLeftY + Height;
        ++Y)
    {
        for(s32 X = TopLeftX;
            X < TopLeftX + Width;
            ++X)
        {
            r32 *Pixel = (r32 *)ZBuffer->Memory + (Y*ZBuffer->Width + X);
    
            *Pixel = -1000.0f;
        }
    }    
}

internal void
DrawRectangle(game_back_buffer *Buffer,
              s32 TopLeftX, s32 TopLeftY, s32 Width, s32 Height,
              r32 Red, r32 Green, r32 Blue)
{
    for(s32 Y = TopLeftY;
        Y < TopLeftY + Height;
        ++Y)
    {
        for(s32 X = TopLeftX;
            X < TopLeftX + Width;
            ++X)
        {
            DrawPixel(Buffer, X, Y, Red, Green, Blue);
        }
    }
}

internal void
DrawLine(game_back_buffer *Buffer,
         r32 X0, r32 Y0,
         r32 X1, r32 Y1,
         r32 Red, r32 Green, r32 Blue)
{
    r32 Dx = X1 - X0;
    r32 Dy = Y1 - Y0;

    s32 Steps = 0;
    
    if(AbsoluteValue(Dx) > AbsoluteValue(Dy))
    {
        Steps = (s32)AbsoluteValue(Dx);
    }
    else
    {
        Steps = (s32)AbsoluteValue(Dy);
    }

    r32 xIncrement = Dx / (r32)Steps;
    r32 yIncrement = Dy / (r32)Steps;

    r32 X = X0;
    r32 Y = Y0;
    for(s32 V = 0;
        V < Steps;
        ++V)
    {
        X += xIncrement;
        Y += yIncrement;
        DrawPixel(Buffer, (s32)X, (s32)Y, Red, Green, Blue);        
    }
}

internal void
PerspectiveProjectV3(game_back_buffer *Buffer, v3 *Vertex,
                     r32 FocalLength, r32 CameraZ, r32 MetersToPixels)
{
    Vertex->x = (FocalLength*Vertex->x) / (CameraZ - Vertex->z);    
    Vertex->y = (FocalLength*Vertex->y) / (CameraZ - Vertex->z);

    Vertex->x *= MetersToPixels;
    Vertex->y *= MetersToPixels;

    Vertex->x += (s32)((r32)Buffer->Width*0.5f);
    Vertex->y += (s32)((r32)Buffer->Height*0.5f);
}

internal void
PerspectiveProjectTriangle(game_back_buffer *Buffer, triangle *Triangle,
                           r32 FocalLength, r32 CameraZ, r32 MetersToPixels)
{
    PerspectiveProjectV3(Buffer, &Triangle->V[0].P, FocalLength, CameraZ, MetersToPixels);
    PerspectiveProjectV3(Buffer, &Triangle->V[1].P, FocalLength, CameraZ, MetersToPixels);
    PerspectiveProjectV3(Buffer, &Triangle->V[2].P, FocalLength, CameraZ, MetersToPixels);
    PerspectiveProjectV3(Buffer, &Triangle->V[0].N, FocalLength, CameraZ, MetersToPixels);
    PerspectiveProjectV3(Buffer, &Triangle->V[1].N, FocalLength, CameraZ, MetersToPixels);
    PerspectiveProjectV3(Buffer, &Triangle->V[2].N, FocalLength, CameraZ, MetersToPixels);
}

internal triangle
ClipTriangleToAxisPlane1(vertex BackVertex, vertex MiddleVertex, vertex FrontVertex,
                         r32 AxisValueBack, r32 AxisValueMiddle, r32 AxisValueFront,
                         r32 PlaneAxisValue)
{
    triangle Result = {};
                            
    r32 t0 = ((PlaneAxisValue - AxisValueBack) /
              (AxisValueFront - AxisValueBack));
    r32 t1 = ((PlaneAxisValue - AxisValueBack) /
              (AxisValueMiddle - AxisValueBack));
    
    vertex NewV0 = BackVertex +
        ((FrontVertex - BackVertex) * t0);
    vertex NewV1 = BackVertex +
        ((MiddleVertex - BackVertex) * t1);

    Result.V[0] = BackVertex;
    Result.V[1] = NewV0;
    Result.V[2] = NewV1;

    return(Result);
}
    
internal clipped_triangle
ClipTriangleToAxisPlane2(vertex BackVertex, vertex MiddleVertex, vertex FrontVertex,
                         r32 AxisValueBack, r32 AxisValueMiddle, r32 AxisValueFront,
                         r32 PlaneAxisValue)
{
    clipped_triangle Result = {};
    
    r32 t0 = ((PlaneAxisValue - AxisValueBack) /
              (AxisValueFront - AxisValueBack));
    r32 t1 = ((PlaneAxisValue - AxisValueMiddle) /
              (AxisValueFront - AxisValueMiddle));
    
    vertex NewV0 = BackVertex +
        ((FrontVertex - BackVertex) * t0);
    vertex NewV1 = MiddleVertex +
        ((FrontVertex - MiddleVertex) * t1);
                            
    Result.TriangleA.V[0] = BackVertex;
    Result.TriangleA.V[1] = MiddleVertex;
    Result.TriangleA.V[2] = NewV0;
                            
    Result.TriangleB.V[0] = MiddleVertex;
    Result.TriangleB.V[1] = NewV0;
    Result.TriangleB.V[2] = NewV1;
    
    return(Result);
}

internal r32
CalculateGouraudLightingForVertex(v3 P, v3 N, light Light)
{
    r32 Result;
    
    v3 VertToCam;
    VertToCam = P;
    v3 VertNorm;
    VertNorm = N - P;
    VertNorm = Normalize(VertNorm);
    VertToCam = Normalize(VertToCam);

    v3 Reflection = {};
    
    r32 Angle = Inner(VertNorm, Light.Direction);
    r32 AbsAngle = AbsoluteValue(Angle);

    Reflection.x = 2 * (Angle) * VertNorm.x - Light.Direction.x;
    Reflection.y = 2 * (Angle) * VertNorm.y - Light.Direction.y;
    Reflection.z = 2 * (Angle) * VertNorm.z - Light.Direction.z;
    
    r32 RV = Inner(Reflection, VertToCam);

    if(RV < 0)
    {
        RV = 0;
    }

    r32 SpecValue = Light.Ks * Pow(RV, Light.Alpha);

    if(SpecValue < 0)
    {
        SpecValue = 0;
    }
    
    Result = (Light.Ka + (Light.Kd * AbsAngle) + SpecValue);
    
    return(Result);
}

internal void
DrawRenderElements(memory_arena *Arena)
{
    r32 FocalLength = 0.6f;
    r32 CameraZ = 0;
    r32 MonitorMeterWidth = 0.635f;
    r32 MonitorPixelWidth = 1920;
    r32 MetersToPixels = MonitorPixelWidth*MonitorMeterWidth;
    r32 NearClipZ = -1.0f;
    r32 FarClipZ = -100.0f;
                
    u32 Index = 0;
    while(Index < Arena->Used)
    {
        render_element_type *ElementType = (render_element_type *)((u08 *)Arena->Base + Index);
    
        switch(*ElementType)
        {
            case RenderElementType_empty:
            {

            } break;

            case RenderElementType_render_element_clear:
            {
                // TODO(tucker): Better way to clear screen
                render_element_clear *Element = (render_element_clear *)((u08 *)Arena->Base + Index);

                DrawRectangle(Element->Buffer, 0, 0,
                              Element->Buffer->Width, Element->Buffer->Height,
                              0.4f, 0.8f, 0.8f);
                ResetZBuffer(Element->ZBuffer);
                
                Index += sizeof(render_element_clear);
            } break;
            
            case RenderElementType_render_element_triangle:
            {
                render_element_triangle *Element = (render_element_triangle *)((u08 *)Arena->Base + Index);
                
                r32 CameraClipTestX = FocalLength / ((Element->Buffer->Width / MetersToPixels)*0.5f);
                r32 CameraClipTestY = FocalLength / ((Element->Buffer->Height / MetersToPixels)*0.5f);
                r32 ClipTest0 = AbsoluteValue(Element->Triangle.V[0].z / Element->Triangle.V[0].x);
                r32 ClipTest1 = AbsoluteValue(Element->Triangle.V[1].z / Element->Triangle.V[1].x);
                r32 ClipTest2 = AbsoluteValue(Element->Triangle.V[2].z / Element->Triangle.V[2].x);
                r32 ClipTest3 = AbsoluteValue(Element->Triangle.V[0].z / Element->Triangle.V[0].y);
                r32 ClipTest4 = AbsoluteValue(Element->Triangle.V[1].z / Element->Triangle.V[1].y);
                r32 ClipTest5 = AbsoluteValue(Element->Triangle.V[2].z / Element->Triangle.V[2].y);
                
                // TODO(tucker): Do we clip when we are on the plane? CameraClipTest = ClipTest
                if((ClipTest0 >= CameraClipTestX ||
                    ClipTest1 >= CameraClipTestX ||
                    ClipTest2 >= CameraClipTestX) &&
                   (ClipTest3 >= CameraClipTestY ||
                    ClipTest4 >= CameraClipTestY ||
                    ClipTest5 >= CameraClipTestY) &&
                   Element->Triangle.V[0].z > FarClipZ &&
                   Element->Triangle.V[1].z > FarClipZ &&
                   Element->Triangle.V[2].z > FarClipZ &&
                   (Element->Triangle.V[0].z < NearClipZ ||
                    Element->Triangle.V[1].z < NearClipZ ||
                    Element->Triangle.V[2].z < NearClipZ))
                {
                    triangle *Triangle = &Element->Triangle;
                    
                    s32 FrontIndex = 0;
                    s32 MiddleIndex = 1;
                    s32 BackIndex = 2;
    
                    if(Element->Triangle.V[FrontIndex].P.z < Element->Triangle.V[MiddleIndex].P.z)
                    {
                        SwapS32(&FrontIndex, &MiddleIndex);
                    }
    
                    if(Element->Triangle.V[FrontIndex].P.z < Element->Triangle.V[BackIndex].P.z)
                    {
                        SwapS32(&FrontIndex, &BackIndex);
                    }
    
                    if(Element->Triangle.V[MiddleIndex].P.z < Element->Triangle.V[BackIndex].P.z)
                    {
                        SwapS32(&MiddleIndex, &BackIndex);
                    }

                    if(Element->Triangle.V[FrontIndex].P.z >= NearClipZ)
                    {
                        if(Element->Triangle.V[MiddleIndex].P.z >= NearClipZ)
                        {
                            triangle ClippedTriangle = ClipTriangleToAxisPlane1(Triangle->V[BackIndex],
                                                                                Triangle->V[MiddleIndex],
                                                                                Triangle->V[FrontIndex],
                                                                                Triangle->V[BackIndex].z,
                                                                                Triangle->V[MiddleIndex].z,
                                                                                Triangle->V[FrontIndex].z,
                                                                                NearClipZ);

                            PerspectiveProjectTriangle(Element->Buffer, &ClippedTriangle,
                                                       FocalLength, CameraZ, MetersToPixels);
                            DrawTriangle(Element->Buffer, &ClippedTriangle, Element->MipMap,
                                         Element->ZBuffer, Element->Tint);
                        }
                        else
                        {
                            // TODO(tucker): Clipping when z is positive

                            clipped_triangle ClippedTriangle =
                                ClipTriangleToAxisPlane2(Triangle->V[BackIndex],
                                                         Triangle->V[MiddleIndex],
                                                         Triangle->V[FrontIndex],
                                                         Triangle->V[BackIndex].z,
                                                         Triangle->V[MiddleIndex].z,
                                                         Triangle->V[FrontIndex].z,
                                                         NearClipZ);
                            
                            PerspectiveProjectTriangle(Element->Buffer, &ClippedTriangle.TriangleA,
                                                       FocalLength, CameraZ, MetersToPixels);
                            PerspectiveProjectTriangle(Element->Buffer, &ClippedTriangle.TriangleB,
                                                       FocalLength, CameraZ, MetersToPixels);
                            DrawTriangle(Element->Buffer, &ClippedTriangle.TriangleA, Element->MipMap,
                                         Element->ZBuffer, Element->Tint);
                            DrawTriangle(Element->Buffer, &ClippedTriangle.TriangleB, Element->MipMap,
                                         Element->ZBuffer, Element->Tint);
                        }
                    }
                    else
                    {
                        PerspectiveProjectTriangle(Element->Buffer, &Element->Triangle,
                                                   FocalLength, CameraZ, MetersToPixels);
                        DrawTriangle(Element->Buffer, &Element->Triangle, Element->MipMap,
                                     Element->ZBuffer, Element->Tint);
                    }
                }
                
                Index += sizeof(render_element_triangle);
            } break;

            case RenderElementType_render_element_outline_triangle:
            {
                render_element_outline_triangle *Element = (render_element_outline_triangle *)((u08 *)Arena->Base + Index);

                triangle *Triangle = &Element->Triangle;

                PerspectiveProjectTriangle(Element->Buffer, Triangle,
                                           FocalLength, CameraZ, MetersToPixels);
                    
                DrawLine(Element->Buffer,
                         Triangle->V[0].x, Triangle->V[0].y,
                         Triangle->V[1].x, Triangle->V[1].y,
                         0, 1.0f, 0);

                DrawLine(Element->Buffer,
                         Triangle->V[1].x, Triangle->V[1].y,
                         Triangle->V[2].x, Triangle->V[2].y,
                         0, 1.0f, 0);
                
                DrawLine(Element->Buffer,
                         Triangle->V[2].x, Triangle->V[2].y,
                         Triangle->V[0].x, Triangle->V[0].y,
                         0, 1.0f, 0);
                              
                Index += sizeof(render_element_outline_triangle);
            } break;
        }
    }
}

internal void
Clear(game_back_buffer *Buffer, memory_arena *Arena, game_back_buffer *ZBuffer)
{
    render_element_clear *Clear = PushStruct(Arena, render_element_clear);

    Clear->RenderElementType = RenderElementType_render_element_clear;
    Clear->Buffer = Buffer;
    Clear->ZBuffer = ZBuffer;
}

internal void
PushTriangle(game_back_buffer *Buffer, memory_arena *Arena, triangle *Triangle)
{
    render_element_triangle *LoadedTriangle = PushStruct(Arena, render_element_triangle);

    LoadedTriangle->RenderElementType = RenderElementType_render_element_triangle;
    LoadedTriangle->Buffer = Buffer;
    LoadedTriangle->Triangle = *Triangle;
}

internal void
PushTriangle(game_back_buffer *Buffer, memory_arena *Arena, render_entity_basis *EntityBasis,
             render_entity_basis *CameraEntBasis,
             vertex *V0, vertex *V1, vertex *V2, mip_map *MipMap, game_back_buffer *ZBuffer, light Light, v4 Tint)
{
    render_entity_basis EBasis = *EntityBasis;

    v3 WV0 = (V0->x*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
        (V0->y*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
        (V0->z*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
    v3 WV1 = (V1->x*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
        (V1->y*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
        (V1->z*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
    v3 WV2 = (V2->x*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
        (V2->y*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
        (V2->z*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
    v3 WN0 = (V0->Nx*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
        (V0->Ny*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
        (V0->Nz*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
    v3 WN1 = (V1->Nx*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
        (V1->Ny*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
        (V1->Nz*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
    v3 WN2 = (V2->Nx*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
        (V2->Ny*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
        (V2->Nz*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;

    v3 EdgeA = WV1 - WV0;
    v3 EdgeB = WV2 - WV0;
    v3 FaceNormal = Cross(EdgeA, EdgeB);
    v3 View = CameraEntBasis->Offset - WV0;
    
    r32 BackFaceCullTest = Inner(View, FaceNormal);
    
    if(BackFaceCullTest <= 0)
    {
        V0->Li = CalculateGouraudLightingForVertex(WV0, WN0, Light);
        V1->Li = CalculateGouraudLightingForVertex(WV1, WN1, Light);
        V2->Li = CalculateGouraudLightingForVertex(WV2, WN2, Light);

        if(CameraEntBasis)
        {
            EBasis.Offset -= CameraEntBasis->Offset;
    
            EBasis = RotateEntityBasisAroundOrigin(EBasis,
                                                   -CameraEntBasis->Rotation.x,
                                                   -CameraEntBasis->Rotation.y,
                                                   -CameraEntBasis->Rotation.z);
        }
    
        v3 PosV0 = (V0->x*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
            (V0->y*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
            (V0->z*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
        v3 PosV1 = (V1->x*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
            (V1->y*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
            (V1->z*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
        v3 PosV2 = (V2->x*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
            (V2->y*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
            (V2->z*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
        v3 PosN0 = (V0->Nx*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
            (V0->Ny*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
            (V0->Nz*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
        v3 PosN1 = (V1->Nx*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
            (V1->Ny*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
            (V1->Nz*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;
    
        v3 PosN2 = (V2->Nx*EBasis.Basis.xAxis*EBasis.Basis.ScaleX) +
            (V2->Ny*EBasis.Basis.yAxis*EBasis.Basis.ScaleY) +
            (V2->Nz*EBasis.Basis.zAxis*EBasis.Basis.ScaleZ) + EBasis.Offset;

        render_element_triangle *LoadedTriangle = PushStruct(Arena, render_element_triangle);
        LoadedTriangle->RenderElementType = RenderElementType_render_element_triangle;
        LoadedTriangle->Buffer = Buffer;
        LoadedTriangle->ZBuffer = ZBuffer;
        LoadedTriangle->MipMap = MipMap;
        LoadedTriangle->Triangle.V[0] = *V0;
        LoadedTriangle->Triangle.V[1] = *V1;
        LoadedTriangle->Triangle.V[2] = *V2;

        LoadedTriangle->Triangle.V[0].P = PosV0;
        LoadedTriangle->Triangle.V[1].P = PosV1;
        LoadedTriangle->Triangle.V[2].P = PosV2;
        LoadedTriangle->Triangle.V[0].N = PosN0;
        LoadedTriangle->Triangle.V[1].N = PosN1;
        LoadedTriangle->Triangle.V[2].N = PosN2;
        LoadedTriangle->Tint = Tint;
    }
    else
    {
        s32 Bla = 7;
    }
}

internal void
OutlineTriangle(game_back_buffer *Buffer, memory_arena *Arena, triangle *Triangle)
{
    render_element_outline_triangle *LoadedTriangle = PushStruct(Arena, render_element_outline_triangle);

    LoadedTriangle->RenderElementType = RenderElementType_render_element_outline_triangle;
    LoadedTriangle->Buffer = Buffer;
    LoadedTriangle->Triangle = *Triangle;
}

internal void
PushVertexGroup(game_back_buffer *Buffer, memory_arena *Arena, render_entity_basis *EntityBasis,
                render_entity_basis *CameraEntBasis,
                vertex_group *VertexGroup, mip_map *MipMap,
                game_back_buffer *ZBuffer, light Light, v4 Tint)
{
    for(u32 Index = 0;
        Index < VertexGroup->IndexCount;
        )
    {
        Assert((Index + 3) <= VertexGroup->IndexCount);

        u32 *IndexList = (u32 *)VertexGroup->IndexList + Index;
        
        vertex *V0 = ((vertex *)VertexGroup->VertexList + *(IndexList + 0));
        vertex *V1 = ((vertex *)VertexGroup->VertexList + *(IndexList + 1));
        vertex *V2 = ((vertex *)VertexGroup->VertexList + *(IndexList + 2));

        PushTriangle(Buffer, Arena, EntityBasis, CameraEntBasis, V0, V1, V2, MipMap, ZBuffer, Light, Tint);
                
        Index += 3;
    }
}
