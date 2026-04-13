#include "PlanetComponent.h"
#include <d3dcompiler.h>
#include <cmath>

#include "../Game.h"
#include "../DisplayWin32.h"
#include "CameraComponent.h"
#include "../Helpers/Vertex.h"

#define ORBIT_DISTANCE_MULT 1.3
#define ANGULAR_SPEED_MULT 0.4f

PlanetComponent::PlanetComponent(Game * game, PlanetComponent * parent, float orbitRadius, float angularSpeed, float radius, DirectX::XMFLOAT4 color, PlanetShape shape, float rotationSpeed)
    : GameComponent(game), parent(parent), orbitRadius(orbitRadius* ORBIT_DISTANCE_MULT), angularSpeed(angularSpeed* ANGULAR_SPEED_MULT), radius(radius), color(color), shape(shape), currentPosition(0, 0, 0), currentRotation(0, 0, 0), indexCount(0), rotationSpeed(rotationSpeed* ANGULAR_SPEED_MULT)
{
    worldMatrix = DirectX::XMMatrixIdentity();
}

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
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    game->Device->CreateInputLayout(inputElements, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &layout);

    std::vector<Vertex> verts;
    std::vector<int> inds;

    // Generate mesh based on chosen shape
    if (shape == PlanetShape::Box) {
        // Unit box centered at 0,0,0 with normals and texcoords
        Vertex v[8];
        v[0].Position = { -1.0f, -1.0f, -1.0f }; v[0].Normal = { -1.0f, -1.0f, -1.0f }; v[0].TexCoord = { 0,0 };
        v[1].Position = { -1.0f,  1.0f, -1.0f }; v[1].Normal = { -1.0f,  1.0f, -1.0f }; v[1].TexCoord = { 0,1 };
        v[2].Position = { 1.0f,  1.0f, -1.0f }; v[2].Normal = { 1.0f,  1.0f, -1.0f }; v[2].TexCoord = { 1,1 };
        v[3].Position = { 1.0f, -1.0f, -1.0f }; v[3].Normal = { 1.0f, -1.0f, -1.0f }; v[3].TexCoord = { 1,0 };
        v[4].Position = { -1.0f, -1.0f,  1.0f }; v[4].Normal = { -1.0f, -1.0f,  1.0f }; v[4].TexCoord = { 0,0 };
        v[5].Position = { -1.0f,  1.0f,  1.0f }; v[5].Normal = { -1.0f,  1.0f,  1.0f }; v[5].TexCoord = { 0,1 };
        v[6].Position = { 1.0f,  1.0f,  1.0f }; v[6].Normal = { 1.0f,  1.0f,  1.0f }; v[6].TexCoord = { 1,1 };
        v[7].Position = { 1.0f, -1.0f,  1.0f }; v[7].Normal = { 1.0f, -1.0f,  1.0f }; v[7].TexCoord = { 1,0 };

        for (int i = 0; i < 8; ++i) verts.push_back(v[i]);

        inds = {
            0,1,2, 0,2,3, 4,6,5, 4,7,6, 4,5,1, 4,1,0,
            3,2,6, 3,6,7, 1,5,6, 1,6,2, 4,0,3, 4,3,7
        };
    }
    else {
        // Unit sphere generation with normals and texcoords
        int latLines = 20;
        int longLines = 20;

        for (int i = 0; i <= latLines; i++) {
            float phi = i * XM_PI / latLines;
            for (int j = 0; j <= longLines; j++) {
                float theta = j * 2.0f * XM_PI / longLines;

                // Calculate cartesian coordinates
                float x = sin(phi) * cos(theta);
                float y = cos(phi);
                float z = sin(phi) * sin(theta);

                Vertex v;
                v.Position = { x, y, z };
                v.Normal = { x, y, z }; // On a unit sphere, the position is the normal
                v.TexCoord = { (float)j / longLines, (float)i / latLines };
                verts.push_back(v);
            }
        }

        for (int i = 0; i < latLines; i++) {
            for (int j = 0; j < longLines; j++) {
                int first = (i * (longLines + 1)) + j;
                int second = first + longLines + 1;

                inds.push_back(first);
                inds.push_back(second);
                inds.push_back(first + 1);

                inds.push_back(second);
                inds.push_back(second + 1);
                inds.push_back(first + 1);
            }
        }
    }

    indexCount = static_cast<UINT>(inds.size());

    // Create Vertex Buffer
    D3D11_BUFFER_DESC vDesc = {}; vDesc.Usage = D3D11_USAGE_DEFAULT; vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; vDesc.ByteWidth = sizeof(Vertex) * verts.size();
    D3D11_SUBRESOURCE_DATA vData = {}; vData.pSysMem = verts.data();
    game->Device->CreateBuffer(&vDesc, &vData, &vertices);

    // Create Index Buffer
    D3D11_BUFFER_DESC iDesc = {}; iDesc.Usage = D3D11_USAGE_DEFAULT; iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; iDesc.ByteWidth = sizeof(int) * indexCount;
    D3D11_SUBRESOURCE_DATA iData = {}; iData.pSysMem = inds.data();
    game->Device->CreateBuffer(&iDesc, &iData, &indices);

    // Create Constant Buffer
    D3D11_BUFFER_DESC cbDesc = {}; cbDesc.Usage = D3D11_USAGE_DEFAULT; cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; cbDesc.ByteWidth = sizeof(ConstantBufferData);
    game->Device->CreateBuffer(&cbDesc, nullptr, constantBuffer.GetAddressOf());

    CD3D11_RASTERIZER_DESC rastDesc = {}; rastDesc.CullMode = D3D11_CULL_BACK; rastDesc.FillMode = D3D11_FILL_SOLID;
    game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

void PlanetComponent::Update() {
    using namespace DirectX;

    // Calculate current local angle based on time
    float angle = angularSpeed * game->TotalTime;

    // Orbital offset relative to parent
    float localX = orbitRadius * cos(angle);
    float localZ = orbitRadius * sin(angle);

    currentPosition = { localX, 0.0f, localZ };

    // If it's a moon (has a parent), add the parent's current absolute position
    if (parent) {
        XMFLOAT3 pPos = parent->GetPosition();
        currentPosition.x += pPos.x;
        currentPosition.y += pPos.y; // In case the parent tilts or moves up/down in the future
        currentPosition.z += pPos.z;
    }

    currentRotation.y = rotationSpeed * game->TotalTime;

    // Build absolute world matrix
    worldMatrix = XMMatrixScaling(radius, radius, radius) *
        XMMatrixRotationY(currentRotation.y) *
        XMMatrixRotationX(currentRotation.x) *
        XMMatrixRotationZ(currentRotation.z) *
        XMMatrixTranslation(currentPosition.x, currentPosition.y, currentPosition.z);
}

void PlanetComponent::Draw() {
    using namespace DirectX;

    UINT stride = sizeof(Vertex), offset = 0;
    game->Context->RSSetState(rastState.Get());
    game->Context->IASetInputLayout(layout.Get());
    game->Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->Context->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->Context->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), &stride, &offset);
    game->Context->VSSetShader(vertexShader.Get(), nullptr, 0);
    game->Context->PSSetShader(pixelShader.Get(), nullptr, 0);

    // Don't draw if there's no camera configured
    if (!game->MainCamera) return;

    XMMATRIX view = game->MainCamera->GetViewMatrix();
    XMMATRIX proj = game->MainCamera->GetProjectionMatrix();

    ConstantBufferData cb;
    cb.WVP = XMMatrixTranspose(worldMatrix * view * proj);
    cb.Color = color;

    game->Context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
    game->Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    game->Context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    game->Context->DrawIndexed(indexCount, 0, 0);
}

void PlanetComponent::DestroyResources() {
    layout.Reset();   pixelShader.Reset(); vertexShader.Reset();
    vertices.Reset(); indices.Reset();     constantBuffer.Reset(); rastState.Reset();
}

DirectX::XMFLOAT3 PlanetComponent::GetPosition() const {
    return currentPosition;
}

DirectX::XMFLOAT3 PlanetComponent::GetRotation() const {
    return currentRotation;
}