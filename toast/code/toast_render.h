#if !defined(TOAST_RENDER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

struct triangle
{
    vertex V[3];

    u32 I[3];
    vertex *VertexList;
};

struct light
{
    v3 Direction;
    r32 Ka;
    r32 Kd;
    r32 Ks;
    r32 Alpha;
};

struct texture
{
    void *Memory;

    s32 Width;
    s32 Height;
    s32 Pitch;
};

struct mip_map
{
    texture Textures[9];
};

struct clipped_triangle
{
    triangle TriangleA;
    triangle TriangleB;
    s32 TriangleCount;
};

struct vertex_group
{
    u32 VertexCount;
    vertex *VertexList;
    
    u32 IndexCount;
    u32 *IndexList;
};

struct render_basis
{
    v3 xAxis;
    v3 yAxis;
    v3 zAxis;

    r32 ScaleX;
    r32 ScaleY;
    r32 ScaleZ;
};

struct render_entity_basis
{
    render_basis Basis;
    v3 Offset;
    v3 Rotation;
};

enum render_element_type
{
    RenderElementType_empty,
    RenderElementType_render_element_clear,
    RenderElementType_render_element_triangle,
    RenderElementType_render_element_outline_triangle,
    RenderElementType_render_element_entity,
};

struct render_element_clear
{
    render_element_type RenderElementType;

    game_back_buffer *Buffer;
    game_back_buffer *ZBuffer;
};

struct render_element_triangle
{
    render_element_type RenderElementType;

    game_back_buffer *Buffer;
    game_back_buffer *ZBuffer;
    triangle Triangle;
    mip_map *MipMap;
    v4 Tint;
};

struct render_element_outline_triangle
{
    render_element_type RenderElementType;

    game_back_buffer *Buffer;
    triangle Triangle;
};

struct render_element_entity
{
    render_element_type RenderElementType;

    game_back_buffer *Buffer;
    render_entity_basis EntityBasis;
    vertex_group VertexGroup;
};

#define TOAST_RENDER_H
#endif
