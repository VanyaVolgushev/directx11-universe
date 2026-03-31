#include <algorithm>

#include "OrbitalCam.h"
#include "Game.h"
#include "InputDevice.h"
#include "DisplayWin32.h"
#include "PlanetComponent.h"

OrbitalCam::OrbitalCam(Game* game, float initialRadius, PlanetComponent* parent)
    : CameraComponent(game), radius(initialRadius), parent(parent), pitch(0.0f), yaw(0.0f),
    zoomSpeed(50.0f), mouseSensitivity(0.005f), prevTotalTime(0.0f) {
}

void OrbitalCam::Initialize() {
    game->InputDevice->MouseMove.AddRaw(this, &OrbitalCam::OnMouseMove);
    prevTotalTime = game->TotalTime;
}

void OrbitalCam::Update() {
    if (game->MainCamera != this) return;

    float dt = game->DeltaTime;
    if (game->InputDevice->IsKeyDown(Keys::W)) radius -= zoomSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::S)) radius += zoomSpeed * dt;

    // Prevent the camera from crossing the focal point
    if (radius < 0.5f) radius = 0.5f;
}

void OrbitalCam::OnMouseMove(const InputDevice::MouseMoveEventArgs& args) {
    if (game->MainCamera != this) return;

    if (game->InputDevice->IsKeyDown(Keys::RightButton)) {
        yaw += args.Offset.x * mouseSensitivity;
        pitch += args.Offset.y * mouseSensitivity;

        // Clamp pitch to avoid gimbal lock
        pitch = max(-DirectX::XM_PIDIV2 + 0.01f, min(DirectX::XM_PIDIV2 - 0.01f, pitch));
    }
}

DirectX::XMMATRIX OrbitalCam::GetViewMatrix() {
    using namespace DirectX;

    XMVECTOR targetPos = XMVectorZero();
    if (parent) {
        XMFLOAT3 pos = parent->GetPosition();
        targetPos = XMLoadFloat3(&pos);
    }

    XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
    XMVECTOR offset = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, -radius, 0.0f), rot);

    XMVECTOR cameraPos = targetPos + offset;

    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(cameraPos, targetPos, up);
}

DirectX::XMMATRIX OrbitalCam::GetProjectionMatrix() {
    using namespace DirectX;
    float aspect = (float)game->Display->ClientWidth / (float)game->Display->ClientHeight;
    return XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 1000.0f);
}