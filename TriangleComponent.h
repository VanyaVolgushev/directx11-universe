#pragma once
#include "GameComponent.h"
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class TriangleComponent : public GameComponent
{
private:
    ComPtr<ID3D11InputLayout>          layout;
    ComPtr<ID3D11PixelShader>          pixelShader;
    ComPtr<ID3DBlob>                   pixelShaderByteCode;
    ComPtr<ID3D11RasterizerState>      rastState;
    ComPtr<ID3D11VertexShader>         vertexShader;
    ComPtr<ID3DBlob>                   vertexShaderByteCode;

    std::vector<DirectX::XMFLOAT4>     vertices;   // interleaved pos+color

public:
    TriangleComponent() = default;
    explicit TriangleComponent(Game* game);

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void DestroyResources() override;
};