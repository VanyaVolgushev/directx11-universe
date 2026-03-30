#pragma once
#include "CameraComponent.h"
#include "InputDevice.h"
#include <directxmath.h>

class OrbiterCam : public CameraComponent {
public:
    OrbiterCam(Game* game, float initialRadius = 3.0f);

    DirectX::XMMATRIX GetViewMatrix() override;
    DirectX::XMMATRIX GetProjectionMatrix() override;

    void Initialize() override;
    void Update() override;

private:
    float radius;
    float pitch;
    float yaw;
    float zoomSpeed;
    float mouseSensitivity;
    float prevTotalTime;

    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
};