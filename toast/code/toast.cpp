/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

#include "toast.h"
#include "toast_quaternion.cpp"
#include "toast_render.cpp"

internal void
GenerateMipMaps(mip_map *MipMap, texture *Texture)
{
    MipMap->Textures[0] = *Texture;

    texture *SourceT = &MipMap->Textures[0];
    texture *DestT = &MipMap->Textures[1];

    for(s32 MipLevel = 0;
        MipLevel < 8;
        ++MipLevel)
    {
        DestT->Width = SourceT->Width / 2;
        DestT->Height = SourceT->Height / 2;

        s32 DY = 0;
        for(s32 SY = 0;
            SY < SourceT->Height;
            )
        {
            s32 DX = 0;
            for(s32 SX = 0;
                SX < SourceT->Width;
                )
            {
                u32 *TexelDest = (u32 *)DestT->Memory + (DY*DestT->Width + DX);
                
                u32 *TexelA = (u32 *)SourceT->Memory + (SY*SourceT->Width + SX);
                u32 *TexelB = (u32 *)SourceT->Memory + (SY*SourceT->Width + (SX + 1));
                u32 *TexelC = (u32 *)SourceT->Memory + ((SY + 1)*SourceT->Width + SX);
                u32 *TexelD = (u32 *)SourceT->Memory + ((SY + 1)*SourceT->Width + (SX + 1));

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

                v4 AvgAB = Lerp(TexelAV4, 0.5f, TexelBV4);
                v4 AvgCD = Lerp(TexelCV4, 0.5f, TexelDV4);

                v4 AvgABCD = Lerp(AvgAB, 0.5f, AvgCD);

                *TexelDest = (((u32)(AvgABCD.a + 0.5f) << 24) |
                              ((u32)(AvgABCD.r + 0.5f) << 16) |
                              ((u32)(AvgABCD.g + 0.5f) << 8) |
                              ((u32)(AvgABCD.b + 0.5f)));
                
                SX += 2;
                DX++;
            }

            SY += 2;
            DY++;
        }
        
        ++SourceT;
        ++DestT;
    }
}

internal vertex_group
MD2ToVertexGroup(memory_arena *Arena, loaded_md2 *MD2)
{
    vertex_group Result = {};
    
    vertex_group *VertexGroup = PushStruct(Arena, vertex_group);
    vertex *VertexList = PushArray(Arena, vertex, MD2->VertexCount);
    u32 *IndexList = PushArray(Arena, u32, (MD2->TriangleCount)*3);

    Assert(VertexGroup && VertexList && IndexList);

    Result.VertexCount = MD2->VertexCount;
    Result.IndexCount = (MD2->TriangleCount)*3;
    Result.VertexList = VertexList;
    Result.IndexList = IndexList;

    md2_frame *MD2Frame = (md2_frame *)MD2->FrameList;

    vertex *DestV = VertexList;
    md2_vertex *SourceV = MD2->VertexList;
    
    for(u32 Index = 0;
        Index < MD2->VertexCount;
        ++Index)
    {
        DestV->P = {((SourceV->Position[0]*MD2Frame->Scale[0]) + MD2Frame->Translate[0]),
                    ((SourceV->Position[1]*MD2Frame->Scale[1]) + MD2Frame->Translate[1]),
                    ((SourceV->Position[2]*MD2Frame->Scale[2]) + MD2Frame->Translate[2])};
        DestV->Color = V4(1, 1, 1, 1);
        DestV++;
        SourceV++;            
    }

    u32 *DestI = IndexList;
    md2_triangle *SourceI = MD2->TriangleList;

    for(u32 Index = 0;
        Index < MD2->TriangleCount;
        ++Index)
    {
        *DestI++ = SourceI->VertexIndex[0];
        *DestI++ = SourceI->VertexIndex[1];
        *DestI++ = SourceI->VertexIndex[2];
        SourceI++;
    }
    
    for(u32 TriangleIndex = 0;
        TriangleIndex < MD2->TriangleCount;
        ++TriangleIndex)
    {
        md2_triangle *MD2Triangle = (md2_triangle *)MD2->TriangleList + TriangleIndex;

        for(u32 Index = 0;
            Index < 3;
            ++Index)
        {
            vertex *DestUV = (vertex *)VertexList + MD2Triangle->VertexIndex[Index];
            md2_texture_coordinate *SourceUV =
                (md2_texture_coordinate *)MD2->TextureCoordList + MD2Triangle->TextureIndex[Index];

            DestUV->u = ((r32)SourceUV->U) / MD2->SkinWidth;
            DestUV->v = (-((r32)SourceUV->V) / MD2->SkinHeight) + 1.0f;
        }
    }

#if 1
    vertex *TargetVertex = Result.VertexList;
    
    for(u32 TargetIndex = 0;
        TargetIndex < (MD2->TriangleCount*3);
        ++TargetIndex)
    {
        md2_triangle *Triangle = MD2->TriangleList;
        v3 NormalSum = {};
            
        for(u32 TriangleIndex = 0;
            TriangleIndex < MD2->TriangleCount;
            ++TriangleIndex)
        {
            for(u32 Index = 0;
                Index < 3;
                ++Index)
            {
                if(TargetIndex == Triangle->VertexIndex[Index])
                {
                    vertex *V0 = (vertex *)Result.VertexList + Triangle->VertexIndex[0];
                    vertex *V1 = (vertex *)Result.VertexList + Triangle->VertexIndex[1];
                    vertex *V2 = (vertex *)Result.VertexList + Triangle->VertexIndex[2];

                    v3 EdgeA = V1->P - V0->P;
                    v3 EdgeB = V2->P - V0->P;

                    NormalSum += Cross(EdgeB, EdgeA);
                }
            }

            ++Triangle;
        }

        TargetVertex->N = TargetVertex->P + Normalize(NormalSum);
        ++TargetVertex;
    }
#endif
    
    return(Result);
}

internal loaded_md2
LoadMD2(read_entire_file *ReadEntireFile, char *Filename)
{
    loaded_md2 Result = {};
    entire_file ReadResult = ReadEntireFile(Filename);

    if(ReadResult.DataSize > 0)
    {
        md2_header *Header = (md2_header *)ReadResult.Data;

        Result.SkinWidth = Header->SkinWidth;
        Result.SkinHeight = Header->SkinHeight;

        Result.FrameCount = Header->FrameCount;
        Result.TriangleCount = Header->TriangleCount;
        Result.VertexCount = Header->VertexCount;
        Result.TextureCoordCount = Header->TextureCoordCount;
        
        Result.FrameList = (md2_frame *)((u08 *)ReadResult.Data + Header->FrameDataOffset);
        Result.TriangleList = (md2_triangle *)((u08 *)ReadResult.Data + Header->TriangleOffset);
        Result.VertexList = Result.FrameList->VertexList;
        Result.TextureCoordList = (md2_texture_coordinate *)((u08 *)ReadResult.Data + Header->STTextureOffset);
    }

    return(Result);
}

internal u32
BitScanForward(u32 Value)
{
    u32 Result = 0;

    for(u32 Index = 0;
        Index < 32;
        ++Index)
    {
        if(Value & (1 << Index))
        {
            Result = Index;
            break;
        }
    }

    return(Result);
}

internal texture
LoadBitmap(read_entire_file *ReadEntireFile, char *Filename)
{
    texture Texture = {};

    entire_file EntireFile = ReadEntireFile(Filename);
    if(EntireFile.DataSize > 0)
    {
        bitmap_header *Header = (bitmap_header *)EntireFile.Data;

        Texture.Memory = (u32 *)((u08 *)EntireFile.Data + Header->PixelDataOffset);
        Texture.Width = Header->Width;
        Texture.Height = Header->Height;
        Texture.Pitch = Texture.Width*4;

        u32 RedMask = Header->RedMask;
        u32 GreenMask = Header->GreenMask;
        u32 BlueMask = Header->BlueMask;
        u32 AlphaMask = ~(RedMask|GreenMask|BlueMask);

        u32 RedShift = BitScanForward(RedMask);
        u32 GreenShift = BitScanForward(GreenMask);
        u32 BlueShift = BitScanForward(BlueMask);
        u32 AlphaShift = BitScanForward(AlphaMask);
        
        u32 *Pixel = (u32 *)Texture.Memory;
        for(u32 Y = 0;
            Y < Header->Height;
            ++Y)
        {
            for(u32 X = 0;
                X < Header->Width;
                ++X)
            {
                u32 Color = *Pixel;

                v4 Texel = {(r32)((Color & RedMask) >> RedShift),
                            (r32)((Color & GreenMask) >> GreenShift),
                            (r32)((Color & BlueMask) >> BlueShift),
                            (r32)((Color & AlphaMask) >> AlphaShift)};

                Texel = ConvertToLinearLight(Texel);

                Texel.r *= Texel.a;
                Texel.g *= Texel.a;
                Texel.b *= Texel.a;

                Texel = ConvertToRGB(Texel);
                *Pixel++ = (((u32)(Texel.a + 0.5f) << 24) |
                            ((u32)(Texel.r + 0.5f) << 16) |
                            ((u32)(Texel.g + 0.5f) << 8) |
                            ((u32)(Texel.b + 0.5f)));
            }    
        }
    }

    return(Texture);
}

internal entity
Entity(entity_type Type, b32 Updates,
       render_entity_basis EntityBasis, vertex_group *VertexGroup, mip_map *MipMap,
       lod *LOD)
{
    entity Entity;
    Entity.Type = Type;
    Entity.Updates = Updates;
    Entity.EntityBasis = EntityBasis;
    Entity.VertexGroup = VertexGroup;
    Entity.MipMap = MipMap;
    Entity.LOD = LOD;
    
    return(Entity);
}

internal void
AddEntity(permanent_state *PermState, entity Entity)
{
    PermState->EntityList[PermState->EntityCount++] = Entity;
}

internal void
SetColorAtVertexIndex(vertex_group *VertexGroup, u32 Index)
{
    vertex *Vertex = (vertex *)VertexGroup->VertexList;
}

extern "C" GAME_UPDATE(GameUpdate)
{
    permanent_state *PermState = (permanent_state *)Memory->PermArena.Base;
    if(!PermState->Initialized)
    {
        PushStruct(&Memory->PermArena, permanent_state);
        PermState->PermArena = &Memory->PermArena;

        game_back_buffer *ZBuffer = &PermState->ZBuffer;
        ZBuffer->BytesPerPixel = 4;
        ZBuffer->Width = Buffer->Width;
        ZBuffer->Height = Buffer->Height;

        s32 ZBufferMemorySize = (ZBuffer->Width*ZBuffer->Height)*ZBuffer->BytesPerPixel;
        ZBuffer->Memory = PushSize(PermState->PermArena, ZBufferMemorySize);
        ZBuffer->Pitch = ZBuffer->Width*ZBuffer->BytesPerPixel;

        PermState->DefaultBasis = {};
        PermState->DefaultBasis.Basis.xAxis = {1, 0, 0};
        PermState->DefaultBasis.Basis.yAxis = {0, 1, 0};
        PermState->DefaultBasis.Basis.zAxis = {0, 0, 1};
        PermState->DefaultBasis.Basis.ScaleX = 1;
        PermState->DefaultBasis.Basis.ScaleY = 1;
        PermState->DefaultBasis.Basis.ScaleZ = 1;

        render_basis Basis0 = {};
        Basis0.xAxis = {1, 0, 0};
        Basis0.yAxis = {0, 1, 0};
        Basis0.zAxis = {0, 0, 1};
        Basis0.ScaleX = 1;
        Basis0.ScaleY = 1;
        Basis0.ScaleZ = 1;

        render_entity_basis EntityBasis0 = {Basis0};
        EntityBasis0.Offset = V3(0, 0, 0);
        
        PermState->Time = 0;

        PermState->TestBitmap0 = LoadBitmap(Memory->ReadEntireFile, "test.bmp");
        
        //PermState->TestMD2 = LoadMD2(Memory->ReadEntireFile, "tri2.md2");
        PermState->TestMD2 = LoadMD2(Memory->ReadEntireFile, "floor.md2");
        PermState->VG = MD2ToVertexGroup(PermState->PermArena, &PermState->TestMD2);
        
        vertex_group *VertG = PermState->EntityList[0].VertexGroup;
        
        PermState->Camera = Entity(EntityType_entity_camera, true, PermState->DefaultBasis, 0, 0, 0);
        PermState->Camera.EntityBasis.Offset.y = 1.0f;
        PermState->Camera.EntityBasis.Offset.x = 1.0f;
        
        PermState->Camera.EntityBasis.Offset.z = 3.0f;
        AddEntity(PermState, PermState->Camera);

        //PermState->EntityCount++;
        
        PermState->Initialized = true;

        s32 WH = 256;
        s32 Pitch = 1024;
        for(s32 Index = 0;
            Index < 9;
            ++Index)
        {
            PermState->TestMip.Textures[Index].Memory = PushArray(PermState->PermArena, u32, WH*WH);
            PermState->TestMip.Textures[Index].Width = WH;
            PermState->TestMip.Textures[Index].Height = WH;
            PermState->TestMip.Textures[Index].Pitch = Pitch;
            
            WH /= 2;
            Pitch /= 2;
        }
        
        PermState->EntityTextureTest = LoadBitmap(Memory->ReadEntireFile, "grass.bmp");

        entity TestEntity = Entity(EntityType_entity_box, true, EntityBasis0, &PermState->VG,
                                   &PermState->TestMip, 0);
        for(r32 Z = 0;
            Z < 4.0f;
            ++Z)
        {
            for(r32 X = 0;
                X < 3.0f;
                ++X)
            {
                AddEntity(PermState, TestEntity);
                TestEntity.EntityBasis.Offset.x += 2.0f;
            }
            TestEntity.EntityBasis.Offset.x = 0.0f;
            TestEntity.EntityBasis.Offset.z -= 2.0f;
        }

        //GenerateMipMaps(&PermState->IcoMip, &PermState->EntityTextureTest);
        render_entity_basis IcoBasis = {PermState->DefaultBasis};
        PermState->IcoMD2F = LoadMD2(Memory->ReadEntireFile, "icof.md2");
        PermState->IcoMD2 = LoadMD2(Memory->ReadEntireFile, "ico.md2");
        PermState->IcoVGF = MD2ToVertexGroup(PermState->PermArena, &PermState->IcoMD2F);
        PermState->IcoVG = MD2ToVertexGroup(PermState->PermArena, &PermState->IcoMD2);
        PermState->IcoLOD.VertexGroups[0] = &PermState->IcoVGF;
        PermState->IcoLOD.VertexGroups[1] = &PermState->IcoVG;
        IcoBasis.Offset.z = -5.0f;
        IcoBasis.Offset.x = 2.5f;
        IcoBasis.Offset.y = 1.0f;
        
        entity IcoEnt = Entity(EntityType_entity_box, true, IcoBasis, 0,
                               &PermState->TestMip, &PermState->IcoLOD);
        AddEntity(PermState, IcoEnt);
                
        GenerateMipMaps(&PermState->TestMip, &PermState->EntityTextureTest);

        PermState->EntityList[0].EntityBasis.Basis = RotateBasis(PermState->EntityList[0].EntityBasis.Basis, 45.0f, 0, 0);
    }

    transient_state *TranState = (transient_state *)Memory->TranArena.Base;
    memory_arena *TranArena = &Memory->TranArena;
    if(!TranState->Initialized)
    {
        PushStruct(&Memory->TranArena, transient_state);
        TranState->TranArena = &Memory->TranArena;
        TranState->Initialized = true;
    }
    
    PermState->Time += Input->DeltaTime;
    
    memory_arena_state TranArenaState = BeginTempMemory(TranState->TranArena);
    Clear(Buffer, TranArena, &PermState->ZBuffer);

    v3 ddP = {};
    r32 PlayerSpeed = 7.0f;
    r32 Gravity = 1.0f;
    
    if(Input->MoveLeft)
    {
        //PermState->Camera.EntityBasis.Basis = RotateBasis(PermState->Camera.EntityBasis.Basis, 0, Input->DeltaTime*45.0f, 0);
        ddP.x = -1.0f;
    }

    if(Input->MoveRight)
    {
        //PermState->Camera.EntityBasis.Basis = RotateBasis(PermState->Camera.EntityBasis.Basis, 0, -Input->DeltaTime*45.0f, 0);
        ddP.x = 1.0f;
    }

    if(Input->MoveUp)
    {
        ddP.z = -1.0f;
    }

    if(Input->MoveDown)
    {
        ddP.z = 1.0f;
    }

    ddP = Normalize(ddP);
    ddP *= PlayerSpeed;
    //ddP.y -= Gravity;
    ddP += -2.0f*PermState->Camera.dP;

    v3 MoveBy = (0.5f*ddP*Square(Input->DeltaTime) + Input->DeltaTime*PermState->Camera.dP);
#if 0
    v3 Movement = (MoveBy.x*PermState->Camera.EntityBasis.Basis.xAxis) +
        (MoveBy.y*PermState->Camera.EntityBasis.Basis.yAxis) +
        (MoveBy.z*PermState->Camera.EntityBasis.Basis.zAxis) +
        PermState->Camera.EntityBasis.Offset;
#endif
    
    PermState->Camera.EntityBasis.Offset += MoveBy;
    PermState->Camera.dP += ddP*Input->DeltaTime;
    PermState->Camera.EntityBasis.Offset.y = 1.0f;
        
    //PermState->Camera.EntityBasis.Rotation.y -= Input->DeltaTime*45.0f*Input->MoveLeft;
    //PermState->Camera.EntityBasis.Rotation.y += Input->DeltaTime*45.0f*Input->MoveRight;

    light Light = {};

    Light.Direction = V3(0, -1, 0);
    Light.Ka = 0.3f;
    Light.Kd = 0.7f;
    Light.Ks = 5.3f;
    Light.Alpha = 7.5;

    for(u32 EntityIndex = 0;
        EntityIndex < PermState->EntityCount;
        ++EntityIndex)
    {
        entity *Entity = &PermState->EntityList[EntityIndex];
        
        if(Entity->Updates)
        {
            switch(Entity->Type)
            {
                case EntityType_entity_box:
                {
                    if(Entity->VertexGroup)
                    {
#if 1
                        PushVertexGroup(Buffer, TranArena, &Entity->EntityBasis,
                                        &PermState->Camera.EntityBasis, Entity->VertexGroup, Entity->MipMap,
                                        &PermState->ZBuffer, Light, V4(1, 1, 1, 1));
#endif
                    }
                    else if(Entity->LOD)
                    {
                        Entity->EntityBasis.Offset.x = Sin(PermState->Time * 1.5f)*0.5f + 1.5f;
                        Entity->EntityBasis.Basis.ScaleX = Sin(PermState->Time * 1.0f)*2.0f + 2.1f;
                        Entity->EntityBasis.Basis = RotateBasis(Entity->EntityBasis.Basis, Input->DeltaTime*25.0f, 0, 0);
                        Entity->EntityBasis.Basis = RotateBasis(Entity->EntityBasis.Basis, 0, 0, Input->DeltaTime*15.0f);
                        
                        r32 Distance = Length(Entity->EntityBasis.Offset - PermState->Camera.EntityBasis.Offset);

                        v4 Tint = V4(1, 1, 1, 1);
                        Tint.r = Sin(PermState->Time) + 1.0f;
                        Tint.b = Cos(PermState->Time*0.5f) + 1.0f;
                        Tint.b = -Sin(PermState->Time*0.25f) + 1.0f;
                        
                        if(Distance < 8)
                        {
                            PushVertexGroup(Buffer, TranArena, &Entity->EntityBasis,
                                            &PermState->Camera.EntityBasis, Entity->LOD->VertexGroups[0],
                                            Entity->MipMap,
                                            &PermState->ZBuffer, Light, Tint);
                        }
                        else
                        {
                            PushVertexGroup(Buffer, TranArena, &Entity->EntityBasis,
                                            &PermState->Camera.EntityBasis, Entity->LOD->VertexGroups[1],
                                            Entity->MipMap,
                                            &PermState->ZBuffer, Light, Tint);
                        }
                    }
                } break;

                case EntityType_entity_camera:
                {
                    
                } break;
            }
        }
    }
    
    DrawRenderElements(TranState->TranArena);
    EndTempMemory(TranState->TranArena, TranArenaState);

    //Entity->EntityBasis.Basis.ScaleX = 1.0f;
    //Entity->EntityBasis.Offset.z = Sin(PermState->Time)- 8.5f;
    //Entity->EntityBasis.Offset.z = -5.0f;
    //Entity->EntityBasis.Basis = RotateBasis(Entity->EntityBasis.Basis, Input->DeltaTime*25.0f, 0, 0);
    //Entity->EntityBasis.Basis = RotateBasis(Entity->EntityBasis.Basis, 0, Input->DeltaTime*25.0f, 0);
    //Entity->EntityBasis.Basis = RotateBasis(Entity->EntityBasis.Basis, 0, 0, Input->DeltaTime*25.0f);
    //Entity->EntityBasis.Offset.y = -1.0f;
    //Entity->EntityBasis.Offset.x = 0;
    //DrawTexture(Buffer, &PermState->TestMip.Textures[0], 10.0f, 100.0f, V4(1, 1, 1, 1));

    //DrawTexture(Buffer, &PermState->TestBitmap0, Sin(PermState->Time*0.3f)*200 + 170.0f, 100.0f, V4(1, 1, 1, 0.5f));
    //DrawTexture(Buffer, &PermState->TestBitmap1, -Sin(PermState->Time*0.3f)*200 + 170.0f, 100.0f, V4(1, 1, 1, 0.5f));
    //DrawTexture(Buffer, &PermState->TestBitmap2, 170.0f, Sin(PermState->Time*0.3f)*200 + 100.0f, V4(1, 1, 1, 0.5f));
    //DrawTexture(Buffer, &PermState->TestBitmap3, 170.0f, -Sin(PermState->Time*0.3f)*200 + 100.0f, V4(1, 1, 1, 0.5f));
}
