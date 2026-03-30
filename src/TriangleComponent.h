#pragma once
#include "GameComponent.h"
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

class TriangleComponent : public GameComponent
{
private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indices;

    DirectX::XMFLOAT4 points[8];

public:
    TriangleComponent(Game* game);
    virtual ~TriangleComponent();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void DestroyResources() override;
};