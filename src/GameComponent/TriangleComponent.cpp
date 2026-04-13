#include "TriangleComponent.h"
#include "../Game.h"
#include "../DisplayWin32.h"
#include <d3dcompiler.h>
#include <iostream>

TriangleComponent::TriangleComponent(Game* game) : GameComponent(game) {}

TriangleComponent::~TriangleComponent()
{
    DestroyResources();
}

void TriangleComponent::Initialize()
{
    auto res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        nullptr, nullptr, "VSMain", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
        &vertexShaderByteCode, nullptr);

    if (FAILED(res)) {
        MessageBox(game->Display->hWnd, L"MyVeryFirstShader.hlsl Missing or Invalid", L"Missing Shader File", MB_OK);
        return;
    }

    D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
    res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", Shader_Macros, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShaderByteCode, nullptr);

    game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
    game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    game->Device->CreateInputLayout(inputElements, 2, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &layout);

    DirectX::XMFLOAT4 p[8] = {
        DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };
    memcpy(points, p, sizeof(p));

    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = points;
    game->Device->CreateBuffer(&vertexBufDesc, &vertexData, &vertices);

    int inds[] = { 0,1,2, 1,0,3 };
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.ByteWidth = sizeof(int) * std::size(inds);
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = inds;
    game->Device->CreateBuffer(&indexBufDesc, &indexData, &indices);

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

void TriangleComponent::Update() {}

void TriangleComponent::Draw()
{
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };

    game->Context->RSSetState(rastState.Get());
    game->Context->IASetInputLayout(layout.Get());
    game->Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->Context->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->Context->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), strides, offsets);
    game->Context->VSSetShader(vertexShader.Get(), nullptr, 0);
    game->Context->PSSetShader(pixelShader.Get(), nullptr, 0);

    game->Context->DrawIndexed(6, 0, 0);
}

void TriangleComponent::DestroyResources()
{
    layout.Reset();
    pixelShader.Reset();
    pixelShaderByteCode.Reset();
    rastState.Reset();
    vertexShader.Reset();
    vertexShaderByteCode.Reset();
    vertices.Reset();
    indices.Reset();
}