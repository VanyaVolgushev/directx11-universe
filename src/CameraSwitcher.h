#pragma once
#include "GameComponent.h"
#include <vector>

class CameraComponent;

class CameraSwitcher : public GameComponent
{
private:
    std::vector<CameraComponent*> cameras;
    int currentIndex;
    bool tabWasPressed;

public:
    CameraSwitcher(Game* game);

    void Initialize() override;
    void Update() override;
};