#pragma once
#include <d3d11.h>

class Game;

class GameComponent
{
public:
    Game* game = nullptr;

    GameComponent() = default;
    explicit GameComponent(Game* game) : game(game) {}

    virtual ~GameComponent() = default;

    virtual void Initialize() {}
    virtual void Update() {}
    virtual void Draw() {}
    virtual void Reload() {}
    virtual void DestroyResources() {}
};