#pragma once
#include "SimpleMath.h"

#include "GameComponent.h"

using namespace DirectX::SimpleMath;
class FPSCam : public GameComponent
{
public:
    FPSCam(Game* game) : GameComponent(game) {}
    ~FPSCam();

    //  Gamecomponent interface implementation
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;
    void Reload() override;
};

