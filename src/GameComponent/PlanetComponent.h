#pragma once
#include <directxmath.h>
#include "GameComponent.h"
#include "../ITransformProvider.h"

enum class PlanetShape { Box, Sphere };

class PlanetComponent : public GameComponent, public ITransformProvider
{
public:
    PlanetComponent(Game* game,
        PlanetComponent* parent,
        float orbitRadius,
        float angularSpeed,
        float radius,
        DirectX::XMFLOAT4 color,
        PlanetShape shape = PlanetShape::Sphere,
        float rotationSpeed = 0.0f,
        bool instantiateSphereMesh = true,
        bool usePhongShader = true);

    void CreateMeshChild(PlanetShape shape, float radius, Game* game, DirectX::XMFLOAT4& color, bool usePhongShader);

    virtual ~PlanetComponent() = default;

    void Update() override;

    // ITransformProvider implementation
    DirectX::XMFLOAT3 GetPosition() const override { return currentPosition; }
    DirectX::XMFLOAT3 GetRotation() const override { return currentRotation; }

    float GetRadius() const { return radius; }

private:
    PlanetComponent* parent;
    float orbitRadius;
    float angularSpeed;
    float radius;
    float rotationSpeed;

    DirectX::XMFLOAT4 color;
    PlanetShape shape;

    DirectX::XMFLOAT3 currentPosition;
    DirectX::XMFLOAT3 currentRotation;
};