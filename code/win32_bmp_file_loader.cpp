/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Baskakov $
   ======================================================================== */
#include <windows.h>
#include <stdint.h>

//#include "../..//win32_code_templates/win32codetemplates.h"
#include "../../win32_code_templates/win32codetemplates.cpp"

#define global_variable static;
#define internal_func static;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 bool32;

typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

typedef float real32;
typedef double real64;

#define TARGET_CLIENT_RECT_WIDTH 900
#define TARGET_CLIENT_RECT_HEIGHT 600

global_variable bool GlobalRunning = false;

global_variable i32 GlobalClientRectWidth = 0;
global_variable i32 GlobalClientRectHeight = 0;

global_variable HWND GlobalWindowHandle = { };
global_variable HDC GlobalDCHandle = { };
global_variable BITMAPINFO BitmapInfo = { };
global_variable void* GlobalBackbufferMemory = 0;

struct load_entire_file{
    ui32 FileSize = 0;
    void* FileMemory = 0;
};

#pragma pack(push, 1)
struct bitmap_header{
    ui16 FileType;
    ui32 FileSize;
    ui16 Reserved1;
    ui16 Reserved2;
    ui32 BitmapOffset;
    ui32 Size;
    i32 Width;
    i32 Height;
    ui16 Planes;
    ui16 BitsPerPixel;
};
#pragma pack(pop)

struct bmp_file{
    bitmap_header BitmapHeader = { };
    load_entire_file FileContents = { };
    ui32* Pixels = 0;
    
};

internal_func void
Win32_GetWindowRectDim(i32* WindowRectWidth, i32* WindowRectHeight)
{
    RECT Rect = { };
    GetWindowRect(GlobalWindowHandle, &Rect);
    *WindowRectWidth = Rect.right - Rect.left;
    *WindowRectHeight = Rect.bottom - Rect.top;
}

internal_func void
Win32_GetClientRectDim(i32* ClientRectWidth, i32* ClientRectHeight)
{
    RECT Rect = { };
    GetClientRect(GlobalWindowHandle, &Rect);
    *ClientRectWidth = Rect.right - Rect.left;
    *ClientRectHeight = Rect.bottom - Rect.top;
}

internal_func void
Win32_ResizeClientRectToTargetRes(i32 TargetWidth, i32 TargetHeight)
{
    i32 CurrentClientRectWidth = 0;
    i32 CurrentClientRectHeight = 0;

    i32 CurrentWindowRectWidht = 0;
    i32 CurrentWindowRectHeight = 0;

    Win32_GetClientRectDim(&CurrentClientRectWidth, &CurrentClientRectHeight);
    Win32_GetWindowRectDim(&CurrentWindowRectWidht, &CurrentWindowRectHeight);

    i32 ClientRectOffsetWidth = CurrentWindowRectWidht - CurrentClientRectWidth;
    i32 ClientRectOffsetHeight = CurrentWindowRectHeight - CurrentClientRectHeight;

    MoveWindow(GlobalWindowHandle, 0, 0 ,
               TargetWidth + ClientRectOffsetWidth,
               TargetHeight + ClientRectOffsetHeight,
               TRUE);  
}

//TODO: Read entire file
internal_func load_entire_file
Win32_ReadEntireFile(LPCSTR FileName)
{
    load_entire_file RequestedFile = { };
    HANDLE FileHandle = { };
    FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSizeUnion = { };
        if(GetFileSizeEx(FileHandle, &FileSizeUnion))
        {
            ui32 FileSize32 = (ui32)(FileSizeUnion.QuadPart);
            RequestedFile.FileMemory = VirtualAlloc(0, FileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(RequestedFile.FileMemory)
            {
                OutputDebugStringA("File Size > 0\n");
                DWORD BytesToRead = 0;
                if(ReadFile(FileHandle, RequestedFile.FileMemory,
                            FileSize32, &BytesToRead, 0) &&
                   FileSize32 == BytesToRead)
                {
                    RequestedFile.FileSize = FileSize32;
                }
            }
        }
    }
    CloseHandle(FileHandle);
    return RequestedFile;
}
 
internal_func bmp_file
Win32_LoadBMP(char *FileName)
{
    bmp_file BMPFile = { };
    
    BMPFile.FileContents = Win32_ReadEntireFile(FileName);    
    if(BMPFile.FileContents.FileSize != 0)
    {
        bitmap_header *BMPHeader = (bitmap_header *)BMPFile.FileContents.FileMemory;

        BMPFile.BitmapHeader = *BMPHeader;
        
        BMPFile.Pixels = (ui32 *)((ui8 *)BMPFile.FileContents.FileMemory +
                                BMPFile.BitmapHeader.BitmapOffset);
    }   
    //By default sets to 24 bits
    //but msdn says must be set to 32 by image header???
    BMPFile.BitmapHeader.BitsPerPixel = 32;

    return BMPFile;
}

internal_func void
DrawBitmapToBackBuffer(void* BackbufferMemory, bmp_file BMPFile, ui32 DestX, ui32 DestY)
{ 
    ui32* Row = (ui32*)BackbufferMemory;
    ui32 Pitch = GlobalClientRectWidth;
    //Target row calculated from top to bottom of the bmp image file
    ui32* TargetRow = Row + (Pitch * (DestY + BMPFile.BitmapHeader.Height - 1));
    ui32* TargetPixel = TargetRow + DestX;
    
    ui32* BitmapPixelsMemory = BMPFile.Pixels;
    ui32 BMPWidth = BMPFile.BitmapHeader.Width;
    ui32 BMPHeight = BMPFile.BitmapHeader.Height;
    
    for(ui32 Y = 0; Y < BMPHeight; Y++)
    {
        for(ui32 X = 0; X < BMPWidth; X++)
        {
            *TargetPixel = *BitmapPixelsMemory;
            BitmapPixelsMemory++;
            TargetPixel++;         
        }
        TargetPixel = TargetPixel - Pitch - BMPWidth;
    }   
}

LRESULT CALLBACK
WindowMessageHandlerProcedure(HWND WindowHandle, UINT Message,
           WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = { };

    switch(Message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct;
            BeginPaint(WindowHandle, &PaintStruct);

            

            EndPaint(WindowHandle, &PaintStruct);
            OutputDebugStringA("WM_PAINT\n");
        }break;
        
        case WM_SIZE:
        {
            Win32_GetClientRectDim(&GlobalClientRectWidth, &GlobalClientRectHeight);
            OutputDebugStringA("WM_SIZE\n");
        }break;

        case WM_CLOSE:
        {
            GlobalRunning = false;
        }break;
        
        default:
        {
            Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
        }break;
    }

    return Result;
}

int CALLBACK
WinMain(HINSTANCE WindowInstance, HINSTANCE PrevWindowInstance,
        LPSTR CommandLine, int LineArgs)
{   
    WNDCLASSA WindowClass = { };
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = WindowMessageHandlerProcedure;
    WindowClass.hInstance = WindowInstance;
    WindowClass.hCursor = 0;
    WindowClass.lpszClassName = "AsteroidsGameClassName";

    RegisterClass(&WindowClass);
    
    
    GlobalWindowHandle = CreateWindowEx(0,
                                  WindowClass.lpszClassName,
                                  "Win32AsteroidsGame",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  NULL,
                                  NULL,
                                  WindowInstance,
                                  NULL);

    if(GlobalWindowHandle)
    {
        Win32_ResizeClientRectToTargetRes(TARGET_CLIENT_RECT_WIDTH,TARGET_CLIENT_RECT_HEIGHT);
        Win32_GetClientRectDim(&GlobalClientRectWidth, &GlobalClientRectHeight);

        GlobalBackbufferMemory = Win32_GetBitmapMemory(&BitmapInfo, GlobalBackbufferMemory, 4,
                              GlobalClientRectWidth, GlobalClientRectHeight);

        GlobalDCHandle = GetDC(GlobalWindowHandle);

        
        ShowWindow(GlobalWindowHandle, LineArgs);
        GlobalRunning = true;

        //NOTE: Load bmp file only with 24 or 32 bits depth
        //NOTE: 8 bits of depth - no go!!! image will spooky and be stretched
        bmp_file BMPFile = Win32_LoadBMP("../bmp_src/test5.bmp");
        
        while(GlobalRunning)
        {
            MSG Message = { };
            while(PeekMessage(&Message, GlobalWindowHandle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Message);
                 DispatchMessage(&Message);
            }
            
            DrawBitmapToBackBuffer(GlobalBackbufferMemory, BMPFile, 0, 0);
 
            Win32_DrawDIBSectionToScreen(&GlobalDCHandle, 0, 0, GlobalClientRectWidth, GlobalClientRectHeight,
                                         0, 0, GlobalClientRectWidth, GlobalClientRectHeight,
                                         GlobalBackbufferMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
        }
        
        ReleaseDC(GlobalWindowHandle, GlobalDCHandle);
        Win32_ReleaseBitmapMemory(BMPFile.FileContents.FileMemory);
        Win32_ReleaseBitmapMemory(GlobalBackbufferMemory); 
    }

    return 0;
}

