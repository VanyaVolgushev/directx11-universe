#include "OrbiterCam.h"
#include "Game.h"
#include "InputDevice.h"
#include "DisplayWin32.h"
#include <algorithm>

OrbiterCam::OrbiterCam(Game* game, float initialRadius)
    : CameraComponent(game), radius(initialRadius), pitch(0.0f), yaw(0.0f),
    zoomSpeed(5.0f), mouseSensitivity(0.005f), prevTotalTime(0.0f) {
}

void OrbiterCam::Initialize() {
    game->InputDevice->MouseMove.AddRaw(this, &OrbiterCam::OnMouseMove);
    prevTotalTime = game->TotalTime;
}

void OrbiterCam::Update() {
    if (game->MainCamera != this) return;
    float dt = game->TotalTime - prevTotalTime;
    prevTotalTime = game->TotalTime;
    if (dt <= 0.0f) dt = 0.016f;

    // Zoom in and out using W and S
    if (game->InputDevice->IsKeyDown(Keys::W)) radius -= zoomSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::S)) radius += zoomSpeed * dt;

    // Prevent the camera from crossing the focal point
    if (radius < 0.5f) radius = 0.5f;
}

void OrbiterCam::OnMouseMove(const InputDevice::MouseMoveEventArgs& args) {
    if (game->MainCamera != this) return;

    if (game->InputDevice->IsKeyDown(Keys::RightButton)) {
        yaw += args.Offset.x * mouseSensitivity;
        pitch += args.Offset.y * mouseSensitivity;

        // Clamp pitch to avoid gimbal lock
        pitch = max(-DirectX::XM_PIDIV2 + 0.01f, min(DirectX::XM_PIDIV2 - 0.01f, pitch));
    }
}

DirectX::XMMATRIX OrbiterCam::GetViewMatrix() {
    using namespace DirectX;
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

    // Apply rotation to the offset vector
    XMVECTOR pos = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, -radius, 0.0f), rot);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(pos, XMVectorZero(), up);
}

DirectX::XMMATRIX OrbiterCam::GetProjectionMatrix() {
    using namespace DirectX;
    float aspect = (float)game->Display->ClientWidth / (float)game->Display->ClientHeight;
    return XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 1000.0f);
}   