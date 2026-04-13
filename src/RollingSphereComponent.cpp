#include "RollingSphereComponent.h"
#include "PlanetComponent.h"
#include "Game.h"
#include "InputDevice.h"
#include "Keys.h"
#include <cmath>

// Helper function to convert a rotation matrix to Euler angles (Y, X, Z order)
DirectX::XMFLOAT3 MatrixToEulerYXZ(DirectX::CXMMATRIX m)
{
    using namespace DirectX;
    XMFLOAT4X4 mat;
    XMStoreFloat4x4(&mat, m);

    // For a Y * X * Z rotation matrix, m_12 (row 1, col 2) is sin(x)
    float sx = mat._23;
    if (sx > 1.0f) sx = 1.0f;
    if (sx < -1.0f) sx = -1.0f;
    float x = asin(sx);
    float cx = cos(x);

    float y, z;
    if (cx > 0.001f) // Not Gimbal Locked
    {
        y = atan2(-mat._13, mat._33);
        z = atan2(-mat._21, mat._22);
    }
    else // Gimbal Locked
    {
        z = 0.0f;
        y = atan2(mat._31, mat._11);
    }

    return XMFLOAT3(x, y, z);
}

RollingSphereComponent::RollingSphereComponent(Game* game,
    PlanetComponent* planet,
    float sphereRadius,
    float moveSpeed,
    float turnSpeed)
    : GameComponent(game)
    , m_Planet(planet)
    , m_SphereRadius(sphereRadius)
    , m_MoveSpeed(moveSpeed)
    , m_TurnSpeed(turnSpeed)
    , m_LocalDirection(0, 1, 0)         // start at "north pole"
    , m_Forward(1, 0, 0)                // arbitrary tangent
    , m_WorldPosition(0, 0, 0)
    , m_WorldRotation(0, 0, 0)
    , m_RollQuaternion(0, 0, 0, 1)      // identity
{
    using namespace DirectX;
    // Ensure forward is perpendicular to initial direction
    XMVECTOR dir = XMLoadFloat3(&m_LocalDirection);
    XMVECTOR fwd = XMLoadFloat3(&m_Forward);

    // Project forward onto tangent plane
    XMVECTOR tangent = XMVectorSubtract(fwd, XMVectorMultiply(XMVector3Dot(dir, fwd), dir));
    tangent = XMVector3Normalize(tangent);
    XMStoreFloat3(&m_Forward, tangent);
}

void RollingSphereComponent::Initialize()
{
    // Make sure position and rotation immediately lock to the planet's on Frame 1
    UpdateMovement(0.0f);
}

void RollingSphereComponent::Update()
{
    if (!m_Planet) return;
    UpdateMovement(game->DeltaTime);
}

void RollingSphereComponent::UpdateMovement(float deltaTime)
{
    using namespace DirectX;

    float turn = 0.0f;
    float move = 0.0f;

    InputDevice* input = game->InputDevice;
    if (input)
    {
        if (input->IsKeyDown(Keys::Left)) turn = -m_TurnSpeed * deltaTime;
        if (input->IsKeyDown(Keys::Right)) turn = m_TurnSpeed * deltaTime;

        if (input->IsKeyDown(Keys::Up)) move = m_MoveSpeed * deltaTime;
        if (input->IsKeyDown(Keys::Down)) move = -m_MoveSpeed * deltaTime;
    }

    // 1. Rotate forward vector around the radial direction (local up)
    if (turn != 0.0f)
    {
        XMVECTOR radial = XMLoadFloat3(&m_LocalDirection);
        XMMATRIX rot = XMMatrixRotationAxis(radial, turn);
        XMVECTOR newForward = XMVector3TransformNormal(XMLoadFloat3(&m_Forward), rot);
        XMStoreFloat3(&m_Forward, newForward);
    }

    // 2. Move forward/back along the great circle
    if (move != 0.0f)
    {
        float planetRadius = m_Planet->GetRadius();
        float totalRadius = planetRadius + m_SphereRadius;
        float angle = move / totalRadius;

        // Rotation axis = radial × forward (binormal)
        XMVECTOR radial = XMLoadFloat3(&m_LocalDirection);
        XMVECTOR forward = XMLoadFloat3(&m_Forward);
        XMVECTOR axis = XMVector3Cross(radial, forward);
        axis = XMVector3Normalize(axis);

        // Rotate the radial direction around that axis
        XMMATRIX rot = XMMatrixRotationAxis(axis, angle);
        XMVECTOR newRadial = XMVector3TransformNormal(radial, rot);
        XMStoreFloat3(&m_LocalDirection, newRadial);

        // Rotate the forward vector as well to keep it tangent
        XMVECTOR newForward = XMVector3TransformNormal(forward, rot);
        XMStoreFloat3(&m_Forward, newForward);

        // Update rolling local visual rotation
        UpdateRoll(move);
    }

    // 3. Compute absolute world position from planet's world position + rotated local direction
    XMFLOAT3 planetPos = m_Planet->GetPosition();
    XMFLOAT3 pRot = m_Planet->GetRotation();

    // Reconstruct planet's transform matrix matching MeshRenderer Y*X*Z sequence
    XMMATRIX planetRotMat = XMMatrixRotationY(pRot.y) *
        XMMatrixRotationX(pRot.x) *
        XMMatrixRotationZ(pRot.z);

    float totalRadius = m_Planet->GetRadius() + m_SphereRadius;
    XMVECTOR localDir = XMLoadFloat3(&m_LocalDirection);

    // Rotate the internal planet-local direction to absolute world direction
    XMVECTOR worldDir = XMVector3TransformNormal(localDir, planetRotMat);
    XMVECTOR offset = XMVectorScale(worldDir, totalRadius);
    XMVECTOR worldPos = XMLoadFloat3(&planetPos) + offset;
    XMStoreFloat3(&m_WorldPosition, worldPos);

    // 4. Compute absolute world rotation for renderer (Sphere Roll + Planet Transform Spin)
    XMMATRIX localRollMat = XMMatrixRotationQuaternion(XMLoadFloat4(&m_RollQuaternion));
    XMMATRIX combinedRotMat = localRollMat * planetRotMat;

    // Deconstruct accurate composite Euler Angles out of combined matrices
    m_WorldRotation = MatrixToEulerYXZ(combinedRotMat);
}

void RollingSphereComponent::UpdateRoll(float distance)
{
    using namespace DirectX;
    // Rolling: rotate sphere around axis = cross(radial, forward)
    XMVECTOR radial = XMLoadFloat3(&m_LocalDirection);
    XMVECTOR forward = XMLoadFloat3(&m_Forward);
    XMVECTOR axis = XMVector3Cross(radial, forward);
    axis = XMVector3Normalize(axis);

    float angle = distance / m_SphereRadius;
    XMVECTOR rotQuat = XMQuaternionRotationAxis(axis, angle);

    // Accumulate quaternion right-multiplied (current orientation first, then new adjustment)
    XMVECTOR currentQuat = XMLoadFloat4(&m_RollQuaternion);
    XMVECTOR newQuat = XMQuaternionMultiply(currentQuat, rotQuat);
    XMStoreFloat4(&m_RollQuaternion, newQuat);
}

DirectX::XMFLOAT3 RollingSphereComponent::GetPosition() const
{
    return m_WorldPosition;
}

DirectX::XMFLOAT3 RollingSphereComponent::GetRotation() const
{
    return m_WorldRotation;
}