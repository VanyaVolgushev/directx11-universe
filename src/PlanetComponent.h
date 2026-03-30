#pragma once
#include <SimpleMath.h>
#include <wrl.h>
#include <d3d11.h>

#include "GameComponent.h"

using namespace DirectX::SimpleMath;
class PlanetComponent : public GameComponent
{
public:
    PlanetComponent(Game* game, float orbitRadius, float angularSpeed, float radius, DirectX::XMFLOAT4 color) :
        GameComponent(game),
        orbitRadius(orbitRadius),
        angularSpeed(angularSpeed),
        radius(radius),
        color(color) {}

    ~PlanetComponent();

    //  GameComponent interface implementation
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;

    //  Other
    Vector2 GetPosition();

private:
    float orbitRadius = 0;
    float angularSpeed = 0;
    float radius = 0;

    // Rendering
    DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMMATRIX worldMatrix = DirectX::XMMATRIX();

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

