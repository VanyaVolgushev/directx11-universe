#pragma once
#include "GameComponent.h"
#include "ITransformProvider.h"
#include <DirectXMath.h>

class PlanetComponent;
class InputDevice;

class RollingSphereComponent : public GameComponent,
                               public ITransformProvider
{
public:
    RollingSphereComponent(Game* game,
        PlanetComponent* planet,
        float sphereRadius,
        float moveSpeed = 5.0f,
        float turnSpeed = 2.0f);

    void Initialize() override;
    void Update() override;
    void DestroyResources() override {}

    // ITransformProvider
    DirectX::XMFLOAT3 GetPosition() const override;
    DirectX::XMFLOAT3 GetRotation() const override;

private:
    void UpdateMovement(float deltaTime);
    void UpdateRoll(float distance);

    PlanetComponent* m_Planet;
    float m_SphereRadius;
    float m_MoveSpeed;
    float m_TurnSpeed;

    // Current state in planet-local coordinates
    DirectX::XMFLOAT3 m_LocalDirection;   // normalized direction from planet center to sphere
    DirectX::XMFLOAT3 m_Forward;          // unit tangent direction (in planet-local space)

    // World transform (calculated from planet's world position + local direction)
    DirectX::XMFLOAT3 m_WorldPosition;
    DirectX::XMFLOAT3 m_WorldRotation;    // Euler angles (radians) for visual rolling

    // Rolling orientation quaternion (accumulated)
    DirectX::XMFLOAT4 m_RollQuaternion;   // x,y,z,w
};