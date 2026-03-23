#include "TriangleComponent.h"
#include "Game.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

TriangleComponent::TriangleComponent(Game* g) : GameComponent(g)
{
}

void TriangleComponent::Initialize()
{
    auto device = game->Device.Get();

    // Compile shaders
    ComPtr<ID3DBlob> errorBlob;

    D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        nullptr, nullptr, "VSMain", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &vertexShaderByteCode, &errorBlob);

    if (!vertexShaderByteCode)
    {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        return;
    }

    D3D_SHADER_MACRO macros[] = { {"TEST","1"}, {"TCOLOR","float4(0.0f,1.0f,0.0f,1.0f)"}, {nullptr,nullptr} };

    D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        macros, nullptr, "PSMain", "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &pixelShaderByteCode, &errorBlob);

    if (!pixelShaderByteCode)
    {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        return;
    }

    device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(),
        vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);

    device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(),
        pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

    // Input layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc),
        vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(),
        &layout);

    // Simple triangle (two triangles = quad for testing)
    vertices = {
        { 0.5f,  0.5f, 0.5f, 1}, {1,0,0,1},
        {-0.5f, -0.5f, 0.5f, 1}, {0,0,1,1},
        { 0.5f, -0.5f, 0.5f, 1}, {0,1,0,1},
        {-0.5f,  0.5f, 0.5f, 1}, {1,1,1,1},
    };

    // Rasterizer state
    CD3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.CullMode = D3D11_CULL_NONE;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    device->CreateRasterizerState(&rsDesc, &rastState);
}

void TriangleComponent::Update()
{
    // rotation / animation can be added here later
}

void TriangleComponent::Draw()
{
    auto ctx = game->Context.Get();

    UINT stride = sizeof(DirectX::XMFLOAT4) * 2; // pos + color
    UINT offset = 0;

    ctx->IASetInputLayout(layout.Get());
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(vertexShader.Get(), nullptr, 0);
    ctx->PSSetShader(pixelShader.Get(), nullptr, 0);
    ctx->RSSetState(rastState.Get());

    // You would normally bind vertex/index buffers here.
    // For simplicity the original code used immediate data → in real code move vb/ib to members

    // Draw call (6 indices hard-coded as in original)
    ctx->DrawIndexed(6, 0, 0);
}

void TriangleComponent::DestroyResources()
{
    layout.Reset();
    pixelShader.Reset();
    pixelShaderByteCode.Reset();
    rastState.Reset();
    vertexShader.Reset();
    vertexShaderByteCode.Reset();
}