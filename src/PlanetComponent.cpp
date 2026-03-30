#include <d3dcompiler.h>

#include "PlanetComponent.h"
#include "Game.h"
#include "DisplayWin32.h"
#include "CameraComponent.h"

PlanetComponent::~PlanetComponent() {
    DestroyResources();
}

void PlanetComponent::Initialize() {
    using namespace DirectX;
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob;
    D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vsBlob, nullptr);
    D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG, 0, &psBlob, nullptr);

    game->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    game->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    game->Device->CreateInputLayout(inputElements, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &layout);

    // 8 Vertices for a 3D Box
    XMFLOAT4 points[8] = {
        {-0.5f, -0.5f, -0.5f, 1.0f}, {-0.5f,  0.5f, -0.5f, 1.0f},
        { 0.5f,  0.5f, -0.5f, 1.0f}, { 0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f,  0.5f, 1.0f}, {-0.5f,  0.5f,  0.5f, 1.0f},
        { 0.5f,  0.5f,  0.5f, 1.0f}, { 0.5f, -0.5f,  0.5f, 1.0f}
    };

    D3D11_BUFFER_DESC vDesc = {}; vDesc.Usage = D3D11_USAGE_DEFAULT; vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; vDesc.ByteWidth = sizeof(points);
    D3D11_SUBRESOURCE_DATA vData = {}; vData.pSysMem = points;
    game->Device->CreateBuffer(&vDesc, &vData, &vertices);

    // 36 Indices to make 12 triangles (6 square faces)
    int inds[36] = {
        0,1,2, 0,2,3, 4,6,5, 4,7,6, 4,5,1, 4,1,0,
        3,2,6, 3,6,7, 1,5,6, 1,6,2, 4,0,3, 4,3,7
    };

    D3D11_BUFFER_DESC iDesc = {}; iDesc.Usage = D3D11_USAGE_DEFAULT; iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; iDesc.ByteWidth = sizeof(inds);
    D3D11_SUBRESOURCE_DATA iData = {}; iData.pSysMem = inds;
    game->Device->CreateBuffer(&iDesc, &iData, &indices);

    // Create Constant Buffer
    D3D11_BUFFER_DESC cbDesc = {}; cbDesc.Usage = D3D11_USAGE_DEFAULT; cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; cbDesc.ByteWidth = sizeof(ConstantBufferData);
    game->Device->CreateBuffer(&cbDesc, nullptr, constantBuffer.GetAddressOf());

    CD3D11_RASTERIZER_DESC rastDesc = {}; rastDesc.CullMode = D3D11_CULL_BACK; rastDesc.FillMode = D3D11_FILL_SOLID;
    game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

void PlanetComponent::Update() {

}

void PlanetComponent::Draw() {
    using namespace DirectX;

    UINT stride = 16, offset = 0; // 16 bytes because we only send POSITION now
    game->Context->RSSetState(rastState.Get());
    game->Context->IASetInputLayout(layout.Get());
    game->Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->Context->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->Context->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), &stride, &offset);
    game->Context->VSSetShader(vertexShader.Get(), nullptr, 0);
    game->Context->PSSetShader(pixelShader.Get(), nullptr, 0);

    XMMATRIX view = game->MainCamera->GetViewMatrix();
    XMMATRIX proj = game->MainCamera->GetProjectionMatrix();

    ConstantBufferData cb;
    worldMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f);
    cb.WVP = XMMatrixTranspose(worldMatrix * view * proj);
    cb.Color = color;

    game->Context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    game->Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    game->Context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    game->Context->DrawIndexed(36, 0, 0); // Draw the box!
}

void PlanetComponent::DestroyResources() {
    layout.Reset();   pixelShader.Reset(); vertexShader.Reset();
    vertices.Reset(); indices.Reset();     constantBuffer.Reset(); rastState.Reset();
}

Vector2 PlanetComponent::GetPosition()
{
	return Vector2();
}
