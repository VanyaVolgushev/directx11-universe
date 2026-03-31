#include "IsometricCam.h"
#include "Game.h"
#include "InputDevice.h"
#include "DisplayWin32.h"
#include <algorithm>

IsometricCam::IsometricCam(Game* game, float initialViewWidth)
    : CameraComponent(game), viewWidth(initialViewWidth),
    zoomSpeed(5.0f), mouseSensitivity(0.005f), prevTotalTime(0.0f) {

    pitch = 0.6154729f; // asin(1/sqrt(3))
    yaw = DirectX::XM_PIDIV4;
}

void IsometricCam::Initialize() {
    game->InputDevice->MouseMove.AddRaw(this, &IsometricCam::OnMouseMove);
    prevTotalTime = game->TotalTime;
}

void IsometricCam::Update() {
    if (game->MainCamera != this) return;
    float dt = game->TotalTime - prevTotalTime;
    prevTotalTime = game->TotalTime;
    if (dt <= 0.0f) dt = 0.016f;

    if (game->MainCamera != this) return;

    if (game->InputDevice->IsKeyDown(Keys::W)) viewWidth -= zoomSpeed * dt;
    if (game->InputDevice->IsKeyDown(Keys::S)) viewWidth += zoomSpeed * dt;
    if (viewWidth < 1.0f) viewWidth = 1.0f;
}

void IsometricCam::OnMouseMove(const InputDevice::MouseMoveEventArgs& args) {
    if (game->MainCamera != this) return;

    if (game->InputDevice->IsKeyDown(Keys::RightButton)) {
        yaw += args.Offset.x * mouseSensitivity;
        pitch += args.Offset.y * mouseSensitivity;

        pitch = max(-DirectX::XM_PIDIV2 + 0.01f,
                min(DirectX::XM_PIDIV2 - 0.01f,
                    pitch));
    }
}

DirectX::XMMATRIX IsometricCam::GetViewMatrix() const {
    using namespace DirectX;
    float distance = 100.0f;
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
    XMVECTOR pos = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, -distance, 0.0f), rot);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    return XMMatrixLookAtLH(pos, XMVectorZero(), up);
}

DirectX::XMMATRIX IsometricCam::GetProjectionMatrix() const {
    using namespace DirectX;
    float aspect = (float)game->Display->ClientWidth / (float)game->Display->ClientHeight;
    float viewHeight = viewWidth / aspect;
    return XMMatrixOrthographicLH(viewWidth, viewHeight, 0.01f, 1000.0f);
}