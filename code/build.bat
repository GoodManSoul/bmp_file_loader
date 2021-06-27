@echo off

mkdir ..\build
pushd ..\build


cl -Zi ..\code\win32_bmp_file_loader.cpp User32.lib Gdi32.lib Kernel32.lib

popd