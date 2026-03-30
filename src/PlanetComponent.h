#pragma once
#include <directxmath.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>

#include "GameComponent.h"

// Enum to select the rendering shape
enum class PlanetShape { Box, Sphere };

class PlanetComponent : public GameComponent
{
public:
    PlanetComponent(Game* game,
        PlanetComponent* parent,
        float orbitRadius,
        float angularSpeed,
        float radius,
        DirectX::XMFLOAT4 color,
        PlanetShape shape = PlanetShape::Sphere);

    ~PlanetComponent();

    // GameComponent interface implementation
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;

    // Get the absolute world position of this planet
    DirectX::XMFLOAT3 GetPosition() const;

private:
    PlanetComponent* parent; // If null, orbits the center of the world

    float orbitRadius;
    float angularSpeed;
    float radius;
    DirectX::XMFLOAT4 color;
    PlanetShape shape;

    DirectX::XMFLOAT3 currentPosition;
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