// DisplayWin32.h
#pragma once
#include <windows.h>
#include <string>

class Game;

class DisplayWin32
{
public:
    int ClientWidth;
    int ClientHeight;
    HWND hWnd;
    HINSTANCE hInstance;
    WNDCLASSEX wc{};

    DisplayWin32(int w, int h, const std::wstring& title, Game* game);
    ~DisplayWin32();
};