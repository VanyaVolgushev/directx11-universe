#pragma once
#include <WICTextureLoader.h>
#include <directxmath.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <string>

#include "GameComponent.h"
#include "Helpers/Vertex.h"

class PlanetComponent;

class MeshRenderer : public GameComponent
{
public:
    MeshRenderer(Game* game,
                 PlanetComponent* parent,
                 const std::vector<Vertex>& vertices,
                 const std::vector<int>& indices,
                 std::wstring texturePath = L"./Shaders/MyVeryFirstShader.hlsl",
                 std::wstring shaderPath = L"./Shaders/MyVeryFirstShader.hlsl");

    ~MeshRenderer();

    // GameComponent interface
    void Initialize() override;
    void Draw() override;
    void Update() override;
    void DestroyResources() override;

    // Transform controls
    void SetPosition(const DirectX::XMFLOAT3& pos);
    void SetRotation(const DirectX::XMFLOAT3& rot);
    void SetScale(const DirectX::XMFLOAT3& scl);
    void SetColor(const DirectX::XMFLOAT4& c) { color = c; }

private:
    void UpdateWorldMatrix();

    PlanetComponent* parent;

    std::vector<Vertex> initVertices;
    std::vector<int> initIndices;
    std::wstring shaderPath;

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT4 color;

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
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    std::wstring texturePath;
};