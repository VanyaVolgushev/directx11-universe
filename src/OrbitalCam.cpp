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

DirectX::XMMATRIX OrbitalCam::GetViewMatrix() const {
    using namespace DirectX;

    // Get parent's world position (fallback to origin if none)
    XMVECTOR targetPos = XMVectorZero();
    XMMATRIX parentRot = XMMatrixIdentity();
    if (parent) {
        XMFLOAT3 pos = parent->GetPosition();        
        XMFLOAT3 rot = parent->GetRotation();        
        targetPos = XMLoadFloat3(&pos);
        parentRot = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
    }

    XMMATRIX camRot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
    XMVECTOR localOffset = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, -radius, 0.0f), camRot);
    XMVECTOR worldOffset = XMVector3TransformCoord(localOffset, parentRot);
    XMVECTOR cameraPos = targetPos + worldOffset;
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(cameraPos, targetPos, up);
}

DirectX::XMMATRIX OrbitalCam::GetProjectionMatrix() const {
    using namespace DirectX;
    float aspect = (float)game->Display->ClientWidth / (float)game->Display->ClientHeight;
    return XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 1000.0f);
}