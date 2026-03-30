#include "FPSCam.h"
#include "Game.h"
#include "InputDevice.h"
#include "DisplayWin32.h"
#include <algorithm>

FPSCam::FPSCam(Game* game) : CameraComponent(game),
position(0.0f, 15.0f, -20.0f),
yaw(0.0f),
pitch(0.0f),
moveSpeed(10.0f),
mouseSensitivity(0.005f),
prevTotalTime(0.0f) {}

void FPSCam::Initialize() {
    game->InputDevice->MouseMove.AddRaw(this, &FPSCam::OnMouseMove);
    prevTotalTime = game->TotalTime;
}

void FPSCam::Update() {
    using namespace DirectX;

    // Calculate delta time
    float dt = game->TotalTime - prevTotalTime;
    prevTotalTime = game->TotalTime;
    if (dt <= 0.0f) dt = 0.016f; // Fallback

    XMVECTOR pos = XMLoadFloat3(&position);
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

    // Calculate local forward, right, and up directions
    XMVECTOR forward = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), rot);
    XMVECTOR right = XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), rot);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0); // World Up

    // WASD Movement
    if (game->InputDevice->IsKeyDown(Keys::W)) pos += forward * moveSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::S)) pos -= forward * moveSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::D)) pos += right * moveSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::A)) pos -= right * moveSpeed * dt;

    // Up / Down Movement
    if (game->InputDevice->IsKeyDown(Keys::Space)) pos += up * moveSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::LeftShift)) pos -= up * moveSpeed * dt;

    XMStoreFloat3(&position, pos);
}

void FPSCam::OnMouseMove(const InputDevice::MouseMoveEventArgs& args) {
    // Only look around if Right Mouse Button is held
    if (game->InputDevice->IsKeyDown(Keys::RightButton)) {
        yaw += args.Offset.x * mouseSensitivity;
        pitch += args.Offset.y * mouseSensitivity;

        // Prevent camera from flipping over by clamping pitch
        pitch = max(-1.5f, min(1.5f, pitch));
    }
}

DirectX::XMMATRIX FPSCam::GetViewMatrix() {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&position);
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
    XMVECTOR forward = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), rot);
    XMVECTOR up = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), rot);

    return XMMatrixLookAtLH(pos, pos + forward, up);
}

DirectX::XMMATRIX FPSCam::GetProjectionMatrix() {
    using namespace DirectX;
    float aspect = (float)game->Display->ClientWidth / (float)game->Display->ClientHeight;
    return XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 1000.0f);
}

void FPSCam::Draw() {} // Cameras don't draw themselves
void FPSCam::DestroyResources() {}
void FPSCam::Reload() {}