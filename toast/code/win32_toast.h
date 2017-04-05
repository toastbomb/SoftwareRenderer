#if !defined(WIN32_TOAST_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

struct win32_back_buffer
{
    s32 BytesPerPixel;
    s32 Width;
    s32 Height;
    void *Memory;
    s32 Pitch;
    
    BITMAPINFO Info;
};

struct win32_window_dim
{
    s32 Width;
    s32 Height;
};

struct win32_game_dll
{
    HMODULE DLL;
    FILETIME LastWriteTime;

    game_update *GameUpdate;

    b32 IsValid;
};

#define WIN32_TOAST_H
#endif
