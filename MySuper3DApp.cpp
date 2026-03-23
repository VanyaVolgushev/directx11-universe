// MySuper3DApp.cpp
#include <windows.h>
#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    Game game(800, 800, L"MySuper3DApp");

    game.Initialize();
    game.Run();

    return 0;
}

// Alternative: classic main() version (choose one)
int main()
{
    Game game(800, 800, L"MySuper3DApp");

    game.Initialize();
    game.Run();

    return 0;
}