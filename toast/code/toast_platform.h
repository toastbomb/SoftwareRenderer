#if !defined(TOAST_PLATFORM_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    typedef int8_t s08;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;
    typedef s32 b32;

    typedef uint8_t u08;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef size_t memory_size;

    typedef float r32;
    typedef double r64;

#define Pi32 3.14159265359f
#define RadiansToDegrees 180.0f / Pi32
#define DegreesToRadians Pi32 / 180.0f
    
#define internal static 
#define local_persist static 
#define global_variable static

#define Kilobytes(Amount) (1024LL*(Amount))
#define Megabytes(Amount) (1024LL*Kilobytes(Amount))
#define Gigabytes(Amount) (1024LL*Megabytes(Amount))

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

    struct game_back_buffer
    {
        void *Memory;
        s32 Width;
        s32 Height;
        s32 Pitch;
        s32 BytesPerPixel;
    };

    struct memory_arena
    {
        memory_size Size;
        memory_size Used;
        void *Base;

        b32 TempLock;
    };

    struct memory_arena_state
    {
        memory_size Size;
        memory_size Used;
        void *Base;    
    };

    inline void
    InitializeArena(memory_arena *Arena, memory_size Size, void *Base)
    {
        Arena->Base = (u08 *)Base;
        Arena->Size = Size;
        Arena->Used = 0;
    }

    inline memory_arena_state
    BeginTempMemory(memory_arena *Arena)
    {
        memory_arena_state ArenaState;

        ArenaState.Base = Arena->Base;
        ArenaState.Size = Arena->Size;
        ArenaState.Used = Arena->Used;
    
        Arena->Base = ((u08 *)Arena->Base + Arena->Used);
        Arena->Size = Arena->Size - Arena->Used;
        Arena->Used = 0;
        Arena->TempLock = true;

        return(ArenaState);
    }

    inline void
    EndTempMemory(memory_arena *Arena, memory_arena_state ArenaState)
    {
        Arena->Base = ArenaState.Base;
        Arena->Size = ArenaState.Size;
        Arena->Used = ArenaState.Used;
        Arena->TempLock = false;
    }

#define PushSize(Arena, Size) PushSize_(Arena, Size)
#define PushStruct(Arena, Type) (Type *)PushSize_(Arena, sizeof(Type))
#define PushArray(Arena, Type, Count) (Type *)PushSize_(Arena, sizeof(Type)*Count)
    inline void *
    PushSize_(memory_arena *Arena, memory_size Size)
    {
        Assert((Arena->Used + Size) <= Arena->Size);
        void *Pointer = ((u08 *)Arena->Base + Arena->Used);
        Arena->Used += Size;

        return(Pointer);
    }
    
#define RELEASE_ENTIRE_FILE_MEMORY(name) void name(void *Data)
    typedef RELEASE_ENTIRE_FILE_MEMORY(release_entire_file_memory);

    struct entire_file
    {
        u32 DataSize;
        void *Data;
    };
    
#define READ_ENTIRE_FILE(name) entire_file name(char *Filename)
    typedef READ_ENTIRE_FILE(read_entire_file);
    
    struct game_memory
    {
        memory_arena PermArena;
        memory_arena TranArena;

        read_entire_file *ReadEntireFile;
        release_entire_file_memory *ReleaseEntireFileMemory;
    };

    struct game_input
    {
        b32 MoveUp;
        b32 MoveDown;
        b32 MoveLeft;
        b32 MoveRight;

        s32 MouseDeltaX;
        s32 MouseDeltaY;
        
        r32 DeltaTime;
    };

#define GAME_UPDATE(name) void name(game_back_buffer *Buffer, game_memory *Memory, game_input *Input)
    typedef GAME_UPDATE(game_update);

#ifdef __cplusplus
}
#endif


#define TOAST_PLATFORM_H
#endif
