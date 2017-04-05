/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tucker Kolpin $
   $Notice:  $
   ======================================================================== */

#include <windows.h>

#include "toast_platform.h"
#include "win32_toast.h"

global_variable b32 GlobalRunning;
global_variable win32_back_buffer GlobalBackBuffer;
global_variable u64 GlobalPerformanceFreq;

inline FILETIME
Win32GetLastWriteTime(char *Filename)
{
    FILETIME LastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA Data;
    if(GetFileAttributesEx(Filename, GetFileExInfoStandard, &Data))
    {
        LastWriteTime = Data.ftLastWriteTime;
    }

    return(LastWriteTime);
}

internal win32_game_dll
Win32LoadGame()
{
    char *DLLName = "W:\\build\\toast.dll";
    char *TempDLLName = "W:\\build\\toast_temp.dll";
    char *LockName = "W:\\build\\lock.tmp";
    
    win32_game_dll Result = {};
    
    WIN32_FILE_ATTRIBUTE_DATA AttributeData;
    if(!GetFileAttributesEx(LockName, GetFileExInfoStandard, &AttributeData))
    {
        Result.LastWriteTime = Win32GetLastWriteTime(DLLName);

        CopyFile(DLLName, TempDLLName, FALSE);
    
        Result.DLL = LoadLibraryA(TempDLLName);
        if(Result.DLL)
        {
            Result.GameUpdate = (game_update *)GetProcAddress(Result.DLL, "GameUpdate");

            if(Result.GameUpdate)
            {
                Result.IsValid = true;
            }
        }
    }

    if(!Result.IsValid)
    {
        Result.GameUpdate = 0;
    }
    
    return(Result);
}

internal void
Win32UnloadGame(win32_game_dll *Game)
{
    if(Game->DLL)
    {
        FreeLibrary(Game->DLL);
        Game->DLL = 0;
    }

    Game->IsValid = false;
    Game->GameUpdate = 0;
}

internal void
Win32GetInput(game_input *Input)
{
    Input->MoveUp = (GetAsyncKeyState('W') < 0);
    Input->MoveDown = (GetAsyncKeyState('S') < 0);
    Input->MoveLeft = (GetAsyncKeyState('A') < 0);
    Input->MoveRight = (GetAsyncKeyState('D') < 0);

    POINT CursorPosition;
    GetCursorPos(&CursorPosition);

    s32 CursorPosX = CursorPosition.x;
    s32 CursorPosY = CursorPosition.y;

    Input->MouseDeltaX = CursorPosX - Input->MouseDeltaX;
    Input->MouseDeltaY = CursorPosY - Input->MouseDeltaY;
}

internal void
Win32ResizeBackBuffer(win32_back_buffer *Buffer, s32 NewWidth, s32 NewHeight)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->BytesPerPixel = 4;
    Buffer->Width = NewWidth;
    Buffer->Height = NewHeight;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    s32 BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
}

internal void
Win32DisplayBackBuffer(win32_back_buffer *Buffer, HDC DeviceContext,
                       s32 Width, s32 Height)
{
    StretchDIBits(DeviceContext, 0, 0,
                  Width, Height, 0, 0,
                  Buffer->Width, Buffer->Height,
                  Buffer->Memory, &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

internal win32_window_dim
Win32GetWindowDim(HWND Window)
{
    win32_window_dim WindowDim = {};

    RECT Rect;
    GetClientRect(Window, &Rect);

    WindowDim.Width = Rect.right - Rect.left;
    WindowDim.Height = Rect.bottom - Rect.top;

    return(WindowDim);
}

inline LARGE_INTEGER
Win32GetTime(void)
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result);
}

inline r32
Win32GetDeltaSeconds(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    r32 Result;
    Result = (r32)(End.QuadPart - Start.QuadPart) / (r32)(GlobalPerformanceFreq);
    return(Result);
}

RELEASE_ENTIRE_FILE_MEMORY(ReleaseEntireFileMemory)
{
    if(Data)
    {
        VirtualFree(Data, 0, MEM_RELEASE);
    }
}

READ_ENTIRE_FILE(ReadEntireFile)
{
    entire_file EntireFile = {};

    HANDLE FileHandle = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            Assert(FileSize.QuadPart <= 0xFFFFFFFF);
            u32 FileSizeU32 = (u32)FileSize.QuadPart;
            EntireFile.Data = VirtualAlloc(0, FileSizeU32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(EntireFile.Data)
            {
                DWORD BytesRead;
                if(ReadFile(FileHandle, EntireFile.Data, FileSizeU32, &BytesRead, 0) &&
                   (FileSizeU32 == BytesRead))
                {
                    EntireFile.DataSize = FileSizeU32;
                }
                else
                {
                    ReleaseEntireFileMemory(EntireFile.Data);
                    EntireFile.Data = 0;
                }
            } 
        }

        CloseHandle(FileHandle);
    }
    
    return(EntireFile);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM wParam,
                        LPARAM lParam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);

            win32_window_dim WindowDim = Win32GetWindowDim(Window);
            Win32DisplayBackBuffer(&GlobalBackBuffer, DeviceContext,
                                   WindowDim.Width, WindowDim.Height);

            EndPaint(Window, &Paint);
        } break;

        case WM_CREATE:
        {
            // NOTE(tucker): Nothing goes here. Telling windows we
            // don't want any further initialization.
        } break;

        case WM_CLOSE:
        {
            GlobalRunning = false;            
        } break;

        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;

        default:
        {
            Result = DefWindowProc(Window, Message, wParam, lParam);
        } break;
    }

    return(Result);
}

int WINAPI
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = Instance;
    WindowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WindowClass.lpszClassName = "ToastWindowClass";
    WindowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);

    b32 SleepAtOneMS = (timeBeginPeriod(1) == TIMERR_NOERROR);
    LARGE_INTEGER PerformanceFrequency;
    QueryPerformanceFrequency(&PerformanceFrequency);
    GlobalPerformanceFreq = PerformanceFrequency.QuadPart;

    if(RegisterClassEx(&WindowClass))
    {
        HWND Window = CreateWindowEx(0,
                                     WindowClass.lpszClassName,
                                     "Toast Window",
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     640, 480,
                                     0, 0,
                                     Instance,
                                     0);

        if(Window)
        {
            win32_window_dim WindowDim = Win32GetWindowDim(Window);
            Win32ResizeBackBuffer(&GlobalBackBuffer, WindowDim.Width, WindowDim.Height);

            r32 GameFrameRate = 30.0f;
            r32 TargetDeltaTime = 1.0f / GameFrameRate;
            LARGE_INTEGER LastCounter = Win32GetTime();

            u64 PermanentStorageSize = Gigabytes(1);
            u64 TransientStorageSize = Gigabytes(1);
            u64 TotalStorageSize = PermanentStorageSize + TransientStorageSize;
            void *Memory = VirtualAlloc(0, TotalStorageSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

            game_memory GameMemory = {};
            void *PermanentBase = (u08 *)Memory;
            InitializeArena(&GameMemory.PermArena, PermanentStorageSize, PermanentBase); 
            void *TransientBase = ((u08 *)Memory + PermanentStorageSize);
            InitializeArena(&GameMemory.TranArena, TransientStorageSize, TransientBase);

            GameMemory.ReleaseEntireFileMemory = ReleaseEntireFileMemory;
            GameMemory.ReadEntireFile = ReadEntireFile;
            
            HDC DeviceContext = GetDC(Window);

            win32_game_dll Game = Win32LoadGame();

            game_input GameInput = {};
            
            GlobalRunning = true;
            while(GlobalRunning)
            {
                FILETIME WriteTime = Win32GetLastWriteTime("W:\\build\\toast.dll");
                if(CompareFileTime(&WriteTime, &Game.LastWriteTime) != 0)
                {
                    DWORD Attributes = GetFileAttributes("W:\\build\\lock.tmp");

                    if(Attributes == INVALID_FILE_ATTRIBUTES )
                    {
                        Win32UnloadGame(&Game);
                        Game = Win32LoadGame();
                    }
                }
                
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                game_back_buffer Buffer = {};
                Buffer.Memory = GlobalBackBuffer.Memory;
                Buffer.Width = GlobalBackBuffer.Width;
                Buffer.Height = GlobalBackBuffer.Height;
                Buffer.Pitch = GlobalBackBuffer.Pitch;
                Buffer.BytesPerPixel = GlobalBackBuffer.BytesPerPixel;

                GameInput.DeltaTime = TargetDeltaTime;
                Win32GetInput(&GameInput);
                
                Game.GameUpdate(&Buffer, &GameMemory, &GameInput);

                LARGE_INTEGER CurrentCounter = Win32GetTime();
                r32 DeltaSecondsForFrame = Win32GetDeltaSeconds(LastCounter, CurrentCounter);

                if(DeltaSecondsForFrame < TargetDeltaTime)
                {
                    if(SleepAtOneMS)
                    {
                        DWORD SleepTimeInMS = (DWORD)(1000.0f *
                                                    (TargetDeltaTime - DeltaSecondsForFrame));
                        if(SleepTimeInMS > 0)
                        {
                            Sleep(SleepTimeInMS);
                        }
                    }
                }

                LARGE_INTEGER EndCounter = Win32GetTime();
                LastCounter = EndCounter;

                win32_window_dim WindowDimCurrent = Win32GetWindowDim(Window);
                Win32DisplayBackBuffer(&GlobalBackBuffer, DeviceContext,
                                       WindowDimCurrent.Width, WindowDimCurrent.Height);                
            }

            ReleaseDC(Window, DeviceContext);
        }
        else
        {
            
        }
    }
    else
    {
        
    }
    
    return(0);
}
