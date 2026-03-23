#pragma once
#include <windows.h>

class DisplayWin32
{
public:
    int ClientHeight;
    int ClientWidth;
    HINSTANCE hInstance;
    HWND hWnd;
    HMODULE Module;
    WNDCLASSEX wc;

    DisplayWin32(LPCWSTR applicationName, HINSTANCE hInst, int screenWidth, int screenHeight);
    ~DisplayWin32();
};