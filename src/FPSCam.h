#pragma once
#include "CameraComponent.h"
#include "InputDevice.h"
#include <directxmath.h>

class FPSCam : public CameraComponent {
public:
    FPSCam(Game* game);

    DirectX::XMMATRIX GetViewMatrix() override;
    DirectX::XMMATRIX GetProjectionMatrix() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;
    void Reload() override;

private:
    DirectX::XMFLOAT3 position;
    float yaw;
    float pitch;
    float moveSpeed;
    float mouseSensitivity;
    float prevTotalTime;

    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
};