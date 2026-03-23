// MySuper3DApp.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Game.h"
#include "TriangleComponent.h"
#include <iostream>

int main()
{
    // Initialize the main game object which encapsulates win32, d3d11, inputs, and components loop
    Game game(L"My3DApp", 800, 800);

    // Register our application components
    game.Components.push_back(new TriangleComponent(&game));

    // Initialize all systems
    game.Initialize();

    // Start blocking message and render loop
    game.Run();

    std::cout << "Goodbye World!\n";
    return 0;
}