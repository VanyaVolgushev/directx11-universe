#pragma once
#include "CameraComponent.h"
#include "InputDevice.h"
#include <directxmath.h>

class IsometricCam : public CameraComponent {
public:
    IsometricCam(Game* game, float initialViewWidth = 3.0f);

    DirectX::XMMATRIX GetViewMatrix() const override;
    DirectX::XMMATRIX GetProjectionMatrix() const override;

    void Initialize() override;
    void Update() override;

private:
    float viewWidth;
    float pitch;
    float yaw;
    float zoomSpeed;
    float mouseSensitivity;
    float prevTotalTime;

    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
};