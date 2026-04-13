#include <d3dcompiler.h>
#include "MeshRenderer.h"
#include "../Game.h"
#include "CameraComponent.h"
#include "PlanetComponent.h"

MeshRenderer::MeshRenderer(Game* game,
                           ITransformProvider* parent,
                           const std::vector<Vertex>& vertices,
                           const std::vector<int>& indices,
                           std::wstring texturePath,
                           std::wstring shaderPath)
    : GameComponent(game),
    parent(parent),
    initVertices(vertices),
    initIndices(indices),
    texturePath(texturePath),
    shaderPath(shaderPath),
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    indexCount(static_cast<UINT>(indices.size()))
{
    UpdateWorldMatrix();
}

MeshRenderer::~MeshRenderer() {
    DestroyResources();
}

void MeshRenderer::Initialize() {
    using namespace DirectX;

    // Compile Shaders
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob;
    D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vsBlob, nullptr);
    D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG, 0, &psBlob, nullptr);

    game->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    game->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

    // Create Input Layout
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    game->Device->CreateInputLayout(inputElements, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &layout);
    
    // Load Texture
    DirectX::CreateWICTextureFromFile(game->Device.Get(), game->Context.Get(), texturePath.c_str(), nullptr, textureSRV.GetAddressOf());

    // Create Sampler State
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    game->Device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());

    // Load Vertex Buffer
    D3D11_BUFFER_DESC vDesc = {};
    vDesc.Usage = D3D11_USAGE_DEFAULT;
    vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vDesc.ByteWidth = sizeof(Vertex) * initVertices.size();
    D3D11_SUBRESOURCE_DATA vData = {};
    vData.pSysMem = initVertices.data();
    game->Device->CreateBuffer(&vDesc, &vData, &vertexBuffer);

    // Load Index Buffer
    D3D11_BUFFER_DESC iDesc = {};
    iDesc.Usage = D3D11_USAGE_DEFAULT;
    iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    iDesc.ByteWidth = sizeof(int) * indexCount;

    D3D11_SUBRESOURCE_DATA iData = {};
    iData.pSysMem = initIndices.data();
    game->Device->CreateBuffer(&iDesc, &iData, &indexBuffer);

    // Constant Buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(ConstantBufferData);
    game->Device->CreateBuffer(&cbDesc, nullptr, constantBuffer.GetAddressOf());

    // Rasterizer State
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    game->Device->CreateRasterizerState(&rastDesc, &rastState);

    // Free memory of initial data now that it's on the GPU
    initVertices.clear();
    initIndices.clear();
}

void MeshRenderer::Update() {
    if (parent) {
        UpdateWorldMatrix();
    }
}

void MeshRenderer::Draw() {
    if (!game->MainCamera) return;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    game->Context->RSSetState(rastState.Get());
    game->Context->IASetInputLayout(layout.Get());
    game->Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    game->Context->VSSetShader(vertexShader.Get(), nullptr, 0);
    game->Context->PSSetShader(pixelShader.Get(), nullptr, 0);

    // Calculate WVP Matrix
    DirectX::XMMATRIX view = game->MainCamera->GetViewMatrix();
    DirectX::XMMATRIX proj = game->MainCamera->GetProjectionMatrix();

    ConstantBufferData cb;
    cb.WVP = DirectX::XMMatrixTranspose(worldMatrix * view * proj);
    cb.Color = color;

    game->Context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    game->Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    game->Context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    game->Context->PSSetShaderResources(0, 1, textureSRV.GetAddressOf());
    game->Context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    game->Context->DrawIndexed(indexCount, 0, 0);
}

void MeshRenderer::DestroyResources() {
    layout.Reset(); vertexShader.Reset(); pixelShader.Reset();
    vertexBuffer.Reset(); indexBuffer.Reset(); constantBuffer.Reset();
    rastState.Reset();
}

void MeshRenderer::UpdateWorldMatrix() {
    using namespace DirectX;

    // Calculate Local Transform
    XMMATRIX localMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) *
                           XMMatrixRotationY(rotation.y) *
                           XMMatrixRotationX(rotation.x) *
                           XMMatrixRotationZ(rotation.z) *
                           XMMatrixTranslation(position.x, position.y, position.z);
    if (parent) {
        DirectX::XMFLOAT3 pPos = parent->GetPosition();
        DirectX::XMFLOAT3 pRot = parent->GetRotation();

        XMMATRIX parentMatrix = XMMatrixRotationY(pRot.y) *
                                XMMatrixRotationX(pRot.x) *
                                XMMatrixRotationZ(pRot.z) *
                                XMMatrixTranslation(pPos.x, pPos.y, pPos.z);

        worldMatrix = localMatrix * parentMatrix;
    }
    else {
        worldMatrix = localMatrix;
    }
}
void MeshRenderer::SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; UpdateWorldMatrix(); }
void MeshRenderer::SetRotation(const DirectX::XMFLOAT3& rot) { rotation = rot; UpdateWorldMatrix(); }
void MeshRenderer::SetScale(const DirectX::XMFLOAT3& scl) { scale = scl; UpdateWorldMatrix(); }