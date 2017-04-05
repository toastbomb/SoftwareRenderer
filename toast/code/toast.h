#if !defined(TOAST_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

#include "toast_platform.h"
#include "toast_intrinsics.h"
#include "toast_math.h"
#include "toast_render.h"

#pragma pack(push, 1)
struct md2_triangle
{
    u16 VertexIndex[3];
    u16 TextureIndex[3];
};

struct md2_vertex
{
    u08 Position[3];
    u08 LightNormalIndex;
};

struct md2_frame
{
    r32 Scale[3];
    r32 Translate[3];
    u08 Name[16];
    md2_vertex VertexList[1];
};

struct md2_texture_coordinate
{
    u16 U;
    u16 V;
};

struct md2_header
{
    u32 Identity;
    u32 VersionNumber;
    
    u32 SkinWidth;
    u32 SkinHeight;
    
    u32 FrameSize;
    
    u32 TextureCount;
    u32 VertexCount;
    u32 TextureCoordCount;
    u32 TriangleCount;
    u32 OpenGLCommandCount;
    u32 FrameCount;
    
    u32 SkinNameOffset;
    u32 STTextureOffset;
    u32 TriangleOffset;
    u32 FrameDataOffset;
    u32 OpenGLCommandOffset;
    u32 EndOfFileOffest;    
};
#pragma pack(pop)

struct loaded_md2
{
    u32 SkinWidth;
    u32 SkinHeight;
    
    u32 FrameCount;
    u32 TriangleCount;
    u32 VertexCount;
    u32 TextureCoordCount;

    md2_frame *FrameList;
    md2_triangle *TriangleList;
    md2_vertex *VertexList;
    md2_texture_coordinate *TextureCoordList;
};

#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 PixelDataOffset;

    u32 HeaderSize;
    u32 Width;
    u32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 CompressionType;
    u32 SizeOfImage;
    u32 PixelsPerMeterX;
    u32 PixelsPerMeterY;
    u32 ColorsUsedCount;
    u32 SignificantColorCount;

    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
};
#pragma pack(pop)

enum entity_type
{
    EntityType_entity_box,
    EntityType_entity_camera,
};

struct lod
{
    vertex_group *VertexGroups[2];
};

struct entity
{
    entity_type Type;
    b32 Updates;
    render_entity_basis EntityBasis;
    vertex_group *VertexGroup;
    lod *LOD;
    mip_map *MipMap;

    v3 dP;
};

struct permanent_state
{
    b32 Initialized;
    memory_arena *PermArena;
    game_back_buffer ZBuffer;
    
    u32 EntityCount;
    entity EntityList[1000];

    render_entity_basis DefaultBasis;
    entity Camera;

    vertex_group VertexGroup;

    texture TestBitmap0;
    texture TestBitmap1;
    texture TestBitmap2;
    texture TestBitmap3;

    mip_map TestMip;
    
    loaded_md2 TestMD2;
    vertex_group VG;

    texture EntityTextureTest;

    lod IcoLOD;
    loaded_md2 IcoMD2;
    loaded_md2 IcoMD2F;
    vertex_group IcoVG;
    vertex_group IcoVGF;
    mip_map IcoMip;
    
    r32 Time;
};

struct transient_state
{
    b32 Initialized;
    memory_arena *TranArena;
};

#define TOAST_H
#endif
