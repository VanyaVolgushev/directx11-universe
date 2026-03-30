#include <iostream>

#include "PlanetComponent.h"
#include "Game.h"
#include "FPSCam.h"
#include "OrbiterCam.h"
#include "IsometricCam.h"
#include "CameraSwitcher.h"

int main()
{
    Game game(L"PlanetOrbiter", 1024, 768);
    auto sun = new PlanetComponent(&game, nullptr, 0.0f, 0.0f, 3.0f, { 1.0f, 0.9f, 0.0f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(sun);

    auto mercury = new PlanetComponent(&game, sun, 6.0f, 2.5f, 0.4f, { 0.6f, 0.6f, 0.6f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(mercury);

    auto venus = new PlanetComponent(&game, sun, 9.0f, 1.8f, 0.8f, { 0.9f, 0.5f, 0.2f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(venus);

    auto earth = new PlanetComponent(&game, sun, 14.0f, 1.0f, 1.0f, { 0.2f, 0.4f, 0.9f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(earth);

    auto moon = new PlanetComponent(&game, earth, 2.0f, 4.0f, 0.3f, { 0.7f, 0.7f, 0.7f, 1.0f }, PlanetShape::Box);
    game.Components.push_back(moon);

    auto mars = new PlanetComponent(&game, sun, 19.0f, 0.8f, 0.7f, { 0.8f, 0.3f, 0.1f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(mars);

    auto phobos = new PlanetComponent(&game, mars, 1.5f, 5.0f, 0.2f, { 0.5f, 0.4f, 0.4f, 1.0f }, PlanetShape::Box);
    game.Components.push_back(phobos);

    auto jupiter = new PlanetComponent(&game, sun, 28.0f, 0.4f, 2.0f, { 0.8f, 0.7f, 0.6f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(jupiter);

    auto io = new PlanetComponent(&game, jupiter, 2.5f, 3.5f, 0.25f, { 0.9f, 0.9f, 0.2f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(io);
    auto europa = new PlanetComponent(&game, jupiter, 3.2f, 2.8f, 0.2f, { 0.8f, 0.8f, 0.9f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(europa);
    auto ganymede = new PlanetComponent(&game, jupiter, 4.0f, 2.0f, 0.35f, { 0.5f, 0.5f, 0.5f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(ganymede);
    auto callisto = new PlanetComponent(&game, jupiter, 4.8f, 1.5f, 0.3f, { 0.4f, 0.4f, 0.4f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(callisto);

    auto saturn = new PlanetComponent(&game, sun, 38.0f, 0.25f, 1.7f, { 0.9f, 0.8f, 0.5f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(saturn);

    auto titan = new PlanetComponent(&game, saturn, 3.0f, 2.0f, 0.4f, { 0.8f, 0.6f, 0.2f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(titan);
    auto enceladus = new PlanetComponent(&game, saturn, 2.0f, 3.5f, 0.15f, { 0.9f, 0.9f, 0.9f, 1.0f }, PlanetShape::Box);
    game.Components.push_back(enceladus);

    auto uranus = new PlanetComponent(&game, sun, 47.0f, 0.15f, 1.2f, { 0.6f, 0.8f, 0.9f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(uranus);

    auto titania = new PlanetComponent(&game, uranus, 2.0f, 1.5f, 0.2f, { 0.7f, 0.7f, 0.7f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(titania);

    auto neptune = new PlanetComponent(&game, sun, 55.0f, 0.1f, 1.1f, { 0.2f, 0.3f, 0.8f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(neptune);

    auto triton = new PlanetComponent(&game, neptune, 2.0f, -1.8f, 0.25f, { 0.8f, 0.8f, 0.8f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(triton);

    auto pluto = new PlanetComponent(&game, sun, 63.0f, 0.07f, 0.3f, { 0.8f, 0.7f, 0.6f, 1.0f }, PlanetShape::Sphere);
    game.Components.push_back(pluto);

    auto charon = new PlanetComponent(&game, pluto, 0.8f, 2.5f, 0.15f, { 0.6f, 0.6f, 0.6f, 1.0f }, PlanetShape::Box);
    game.Components.push_back(charon);


    game.Components.push_back(new FPSCam(&game));
    game.Components.push_back(new OrbiterCam(&game, 80.0f));
    game.Components.push_back(new IsometricCam(&game, 90.0f));
    game.Components.push_back(new CameraSwitcher(&game));
    game.Initialize();
    game.Run();
    std::cout << "Goodbye World!\n";
    return 0;
}
