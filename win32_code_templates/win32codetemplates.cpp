/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Baskakov $
   ======================================================================== */

#include "win32codetemplates.h"


void* Win32_GetBitmapMemory(BITMAPINFO* BitmapInfo, void* BitmapMemory, int BytesPerPixel,
                            int ClientRectWidth, int ClientRectHeight)
{
   if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitmapInfo->bmiHeader.biWidth = ClientRectWidth;
    BitmapInfo->bmiHeader.biHeight = -ClientRectHeight;
    BitmapInfo->bmiHeader.biPlanes = 1;
    BitmapInfo->bmiHeader.biBitCount = 32;
    BitmapInfo->bmiHeader.biCompression = BI_RGB;
    BitmapInfo->bmiHeader.biSizeImage = 0;
    BitmapInfo->bmiHeader.biXPelsPerMeter = 0; 
    BitmapInfo->bmiHeader.biYPelsPerMeter = 0;
    BitmapInfo->bmiHeader.biClrUsed = 0;
    BitmapInfo->bmiHeader.biClrImportant = 0;
    
    int BitmapMemorySize = ClientRectWidth * ClientRectHeight * BytesPerPixel;
    BitmapMemory = VirtualAlloc(BitmapMemory, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
    return BitmapMemory;    
}

void Win32_ReleaseBitmapMemory(void* BitmapMemory)
{
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }
}

void Win32_ClearBitmapToColor(void* BitmapMemory, int ClientRectWidth, int ClientRectHeight,
                              Pixel32RGB* PixelColor)
{
    int* Row = (int *)BitmapMemory;
    int Pitch = ClientRectWidth;
    for(unsigned int Y = 0; Y < ClientRectHeight; Y++)
    {
        int* Pixel = Row;
        for(unsigned int X = 0; X < ClientRectWidth; X++)
        {
            *Pixel = (PixelColor->Red << 16) | (PixelColor->Green << 8) | (PixelColor->Blue << 0);
            Pixel++;
        }
        Row += Pitch;
    }
}

void Win32_ClearScreenWithPatBlt(HDC *DC, int X, int Y,
                 int ClientRectWidth, int ClientRectHeight, DWORD OperationMode)
{
    PatBlt(*DC, X, Y, ClientRectWidth, ClientRectHeight, OperationMode);
}

void Win32_DrawPixelToBitmap(void* DestBitmapMemory, int ClientRectWidth,
                             int DestX, int DestY, Pixel32RGB* Pixel)
{
    int* Row = (int*)DestBitmapMemory;
    int Pitch = ClientRectWidth;
    int* TargetRow = Row + (Pitch * DestY);
    int* TargetPixel = TargetRow + DestX;
    Pixel32RGB TestPixel;
    TestPixel.Red = 255;
    TestPixel.Green = 255;
    TestPixel.Blue = 255;
    *TargetPixel = (Pixel->Red << 16) | (Pixel->Green << 8) | (Pixel->Blue << 0);
    //*TargetPixel = (TestPixel.Red << 16) | (TestPixel.Green << 8) | (TestPixel.Blue << 0);
}

void Win32_DrawDIBSectionToScreen(HDC *DC, int DestX, int DestY, int DestWidth, int DestHeight,
                                  int SrcX, int SrcY, int SrcWidth, int SrcHeight, void* BitmapMemory,
                                  BITMAPINFO* BitmapInfo, UINT DIBUsage, DWORD OperationMode)
{
    StretchDIBits(*DC,
                  DestX, DestY, DestWidth, DestHeight,
                  SrcX, SrcY, SrcWidth, SrcHeight,
                  BitmapMemory,
                  BitmapInfo,
                  DIBUsage,
                  OperationMode);
}

void Win32_GetClientAreaDimentions(HWND* WindowHandle, int* IN__ClientAreaWidth, int* IN__ClientAreaHeight)
{
    RECT ClientRect = { };
    GetClientRect(*WindowHandle, &ClientRect);
    int ClientWidth = ClientRect.right - ClientRect.left;
    int ClientHeight = ClientRect.bottom - ClientRect.top;

    *IN__ClientAreaWidth = ClientWidth;
    *IN__ClientAreaHeight = ClientHeight;
}



