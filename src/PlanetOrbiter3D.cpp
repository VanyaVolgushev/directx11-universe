#include "Game.h"
#include "PlanetComponent.h"
#include "FPSCam.h"
#include <iostream>

int main()
{
    Game game(L"PlanetOrbiter", 800, 800);
    game.Components.push_back(new PlanetComponent(&game, 0, 0, 100, DirectX::XMFLOAT4{1.0, 1.0, 1.0, 1.0}));
    game.Components.push_back(new FPSCam(&game));
    game.Initialize();
    game.Run();

    std::cout << "Goodbye World!\n";
    return 0;
}
