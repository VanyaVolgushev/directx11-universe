#include <iostream>

#include "PlanetComponent.h"
#include "Game.h"
#include "FPSCam.h"
#include "OrbiterCam.h"
#include "IsometricCam.h"
#include "CameraSwitcher.h"

int main()
{
    Game game(L"PlanetOrbiter", 800, 800);

    // Add planets
    game.Components.push_back(new PlanetComponent(&game, 0, 0, 100, DirectX::XMFLOAT4{1.0, 1.0, 1.0, 1.0}));

    // Add cameras
    game.Components.push_back(new FPSCam(&game));
    game.Components.push_back(new OrbiterCam(&game));
    game.Components.push_back(new IsometricCam(&game));
    game.Components.push_back(new CameraSwitcher(&game));
    game.Initialize();
    game.Run();

    std::cout << "Goodbye World!\n";
    return 0;
}
