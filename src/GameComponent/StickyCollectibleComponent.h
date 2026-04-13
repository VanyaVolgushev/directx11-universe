#pragma once
#include "GameComponent.h"
#include "../ITransformProvider.h"
#include <DirectXMath.h>

class PlanetComponent;
class RollingSphereComponent;

class StickyCollectibleComponent : public GameComponent,
    public ITransformProvider
{
public:
    StickyCollectibleComponent(Game* game,
        PlanetComponent* planet,
        RollingSphereComponent* playerSphere,
        DirectX::XMFLOAT3 localDirection,
        float heightOffset,
        float triggerRadius);

    void Initialize() override;
    void Update() override;
    void DestroyResources() override {}

    // ITransformProvider interface
    DirectX::XMFLOAT3 GetPosition() const override;
    DirectX::XMFLOAT3 GetRotation() const override;

private:
    DirectX::XMFLOAT3 ExtractEulerYXZ(DirectX::CXMMATRIX m);

    PlanetComponent* m_Planet;
    RollingSphereComponent* m_Sphere;

    DirectX::XMFLOAT3 m_LocalDirection; // Direction from planet center
    float m_HeightOffset;               // Distance from planet center
    float m_TriggerRadius;              // Distance at which attachment occurs (Sphere Radius)

    bool m_IsAttached;
    DirectX::XMFLOAT4X4 m_LocalToSphereMat; // Stored relative transform matrix

    DirectX::XMFLOAT3 m_WorldPosition;
    DirectX::XMFLOAT3 m_WorldRotation;
    DirectX::XMMATRIX m_CurrentRotMatrix;
};