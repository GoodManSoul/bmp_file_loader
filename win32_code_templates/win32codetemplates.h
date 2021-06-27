#if !defined(WIN32CODETEMPLATES_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Baskakov $
   ======================================================================== */

#define WIN32CODETEMPLATES_H
#endif

#include <windows.h>

struct Pixel32RGB{
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
};

void* Win32_GetBitmapMemory(BITMAPINFO* BitmapInfo, void* BitmapMemory, int BytesPerPixel,
                            int ClientRectWidth, int ClientRectHeight);

void Win32_ReleaseBitmapMemory(void* BitmapMemory);

void Win32_ClearBitmapToColor(void* BitmapMemory, int ClientRectWidth, int ClientRectHeight,
                              Pixel32RGB* PixelColor);

void Win32_DrawPixelToBitmap(void* DestBitmapMemory, int ClientRectWidht,
                             int DestX, int DestY, Pixel32RGB* Pixel);

void Win32_ClearScreenWithPatBlt(HDC *DC, int X, int Y,
                       int ClientRectWidth, int ClientRectHeight, DWORD OperationMode);

void Win32_DrawDIBSectionToScreen(HDC *DC, int DestX, int DestY, int DestWidht, int DestHeight,
                                  int SrcX, int SrcY, int SrcWidth, int SrcHeight, void* BitmapMemory,
                                  BITMAPINFO* BitmapInfo, UINT DIBUsage, DWORD OperationMode);

void Win32_GetClientAreaDimentions(HWND* WindowHandle, int* IN__ClientAreaWidth, int* IN__ClientAreaHeight);



