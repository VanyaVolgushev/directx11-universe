#pragma once

class Game;

class GameComponent
{
public:
    Game* game;

    GameComponent(Game* game);
    virtual ~GameComponent() = default;

    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    virtual void DestroyResources();
    virtual void Reload();
};