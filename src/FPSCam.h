#pragma once

#include "CameraComponent.h"

class FPSCam : public CameraComponent
{
public:
    FPSCam(Game* game) : CameraComponent(game) {}

    // CameraComponent interface implementation
    DirectX::XMMATRIX GetViewMatrix() override;
    DirectX::XMMATRIX GetProjectionMatrix() override;

    //  Gamecomponent interface implementation
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;
    void Reload() override;
};

