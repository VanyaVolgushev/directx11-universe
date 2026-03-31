#pragma once
#include <directxmath.h>

#include "InputDevice.h"
#include "CameraComponent.h"

class PlanetComponent;

class OrbitalCam : public CameraComponent {
public:
    OrbitalCam(Game* game, float initialRadius = 3.0f, PlanetComponent* parent = nullptr);

    DirectX::XMMATRIX GetViewMatrix() const override;
    DirectX::XMMATRIX GetProjectionMatrix() const override;

    void Initialize() override;
    void Update() override;

private:
    float radius;
    float pitch;
    float yaw;
    float zoomSpeed;
    float mouseSensitivity;
    float prevTotalTime;
    PlanetComponent* parent;

    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
};
