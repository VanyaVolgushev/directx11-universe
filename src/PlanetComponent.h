#pragma once
#include <directxmath.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>

#include "GameComponent.h"
#include "ITransformProvider.h"

enum class PlanetShape { Box, Sphere };

class PlanetComponent : public GameComponent,
                        public ITransformProvider
{
public:
    PlanetComponent(Game* game,
        PlanetComponent* parent,
        float orbitRadius,
        float angularSpeed,
        float radius,
        DirectX::XMFLOAT4 color,
        PlanetShape shape = PlanetShape::Sphere,
        float rotationSpeed = 0.0f);

    ~PlanetComponent();

    // GameComponent interface implementation
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;

    // ITransformProvider interface implementation
    DirectX::XMFLOAT3 GetPosition() const override;
    DirectX::XMFLOAT3 GetRotation() const override;

    // Other
    float GetRadius() const { return radius; }

private:
    PlanetComponent* parent; // If null, orbits the center of the world

    float orbitRadius;
    float angularSpeed;
    float radius;
    float rotationSpeed;

    DirectX::XMFLOAT4 color;
    PlanetShape shape;

    DirectX::XMFLOAT3 currentPosition;
    DirectX::XMFLOAT3 currentRotation;

    DirectX::XMMATRIX worldMatrix;
    UINT indexCount;

    __declspec(align(16))
        struct ConstantBufferData {
        DirectX::XMMATRIX WVP;
        DirectX::XMFLOAT4 Color;
    };

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
};