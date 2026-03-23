// DisplayWin32.cpp
#include "DisplayWin32.h"
#include "Game.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

DisplayWin32::DisplayWin32(int w, int h, const std::wstring& title, Game* game)
    : ClientWidth(w), ClientHeight(h)
{
    hInstance = GetModuleHandle(nullptr);

    wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = title.c_str();

    RegisterClassEx(&wc);

    RECT rect{ 0,0,w,h };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(WS_EX_APPWINDOW,
        title.c_str(), title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, game);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
}

DisplayWin32::~DisplayWin32()
{
    if (hWnd) DestroyWindow(hWnd);
}

// Forward to Game::MessageHandler (you'll need to store Game* in window long ptr or global)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_NCCREATE)
    {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lp);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }

    auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (game)
        game->MessageHandler(hwnd, msg, wp, lp);

    return DefWindowProc(hwnd, msg, wp, lp);
}