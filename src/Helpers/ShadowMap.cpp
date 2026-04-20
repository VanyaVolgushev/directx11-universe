#include "ShadowMap.h"
#include "../Game.h"
#include <d3dcompiler.h>

using namespace DirectX;

struct ShadowFrameCBData
{
    XMFLOAT4 LightPosFarZ; // xyz: light world pos, w: far plane distance
};

ShadowMap::ShadowMap(Game* inGame)
    : game(inGame),
      lightPos{ 0.0f, 0.0f, 0.0f },
      viewport{}
{
    for (int i = 0; i < FACE_COUNT; ++i) {
        faceView[i] = XMMatrixIdentity();
    }
    faceProj = XMMatrixIdentity();
}

void ShadowMap::Initialize()
{
    using namespace Microsoft::WRL;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width      = SHADOW_MAP_SIZE;
    texDesc.Height     = SHADOW_MAP_SIZE;
    texDesc.MipLevels  = 1;
    texDesc.ArraySize  = FACE_COUNT;
    texDesc.Format     = DXGI_FORMAT_R32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage      = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags  = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags  = D3D11_RESOURCE_MISC_TEXTURECUBE;
    game->Device->CreateTexture2D(&texDesc, nullptr, cubeTexture.GetAddressOf());

    for (int i = 0; i < FACE_COUNT; ++i) {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format                         = DXGI_FORMAT_R32_FLOAT;
        rtvDesc.ViewDimension                  = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Texture2DArray.MipSlice        = 0;
        rtvDesc.Texture2DArray.FirstArraySlice = i;
        rtvDesc.Texture2DArray.ArraySize       = 1;
        game->Device->CreateRenderTargetView(cubeTexture.Get(), &rtvDesc, faceRTV[i].GetAddressOf());
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                      = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels       = 1;
    srvDesc.TextureCube.MostDetailedMip = 0;
    game->Device->CreateShaderResourceView(cubeTexture.Get(), &srvDesc, SRV.GetAddressOf());

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width      = SHADOW_MAP_SIZE;
    depthDesc.Height     = SHADOW_MAP_SIZE;
    depthDesc.MipLevels  = 1;
    depthDesc.ArraySize  = 1;
    depthDesc.Format     = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage      = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
    game->Device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
    game->Device->CreateDepthStencilView(depthTexture.Get(), nullptr, depthView.GetAddressOf());

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    game->Device->CreateSamplerState(&sampDesc, Sampler.GetAddressOf());

    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = FALSE;
    rastDesc.DepthClipEnable = TRUE;
    game->Device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

    ComPtr<ID3DBlob> vsBlob, psBlob, errBlob;
    HRESULT hr = D3DCompileFromFile(L"./Shaders/ShadowMapShader.hlsl", nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0",
        D3DCOMPILE_DEBUG, 0, &vsBlob, &errBlob);
    if (FAILED(hr) && errBlob) OutputDebugStringA((const char*)errBlob->GetBufferPointer());

    hr = D3DCompileFromFile(L"./Shaders/ShadowMapShader.hlsl", nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0",
        D3DCOMPILE_DEBUG, 0, &psBlob, &errBlob);
    if (FAILED(hr) && errBlob) OutputDebugStringA((const char*)errBlob->GetBufferPointer());

    game->Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                     nullptr, shadowVS.GetAddressOf());
    game->Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                                    nullptr, shadowPS.GetAddressOf());

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    game->Device->CreateInputLayout(inputElements, 3, vsBlob->GetBufferPointer(),
                                    vsBlob->GetBufferSize(), Layout.GetAddressOf());

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(ShadowFrameCBData);
    game->Device->CreateBuffer(&cbDesc, nullptr, shadowFrameCB.GetAddressOf());

    viewport.Width = static_cast<float>(SHADOW_MAP_SIZE);
    viewport.Height = static_cast<float>(SHADOW_MAP_SIZE);
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

void ShadowMap::UpdateLightMatrices(const XMFLOAT3& inLightPos, float inNearZ, float inFarZ)
{
    lightPos = inLightPos;
    nearZ = inNearZ;
    farZ  = inFarZ;

    XMVECTOR eye = XMLoadFloat3(&lightPos);

    // D3D11_TEXTURECUBE_FACE order: +X, -X, +Y, -Y, +Z, -Z.
    const XMVECTOR forwards[FACE_COUNT] = {
        XMVectorSet( 1.0f,  0.0f,  0.0f, 0.0f),
        XMVectorSet(-1.0f,  0.0f,  0.0f, 0.0f),
        XMVectorSet( 0.0f,  1.0f,  0.0f, 0.0f),
        XMVectorSet( 0.0f, -1.0f,  0.0f, 0.0f),
        XMVectorSet( 0.0f,  0.0f,  1.0f, 0.0f),
        XMVectorSet( 0.0f,  0.0f, -1.0f, 0.0f),
    };
    const XMVECTOR ups[FACE_COUNT] = {
        XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f,  1.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f),
    };

    for (int i = 0; i < FACE_COUNT; ++i) {
        XMVECTOR target = XMVectorAdd(eye, forwards[i]);
        faceView[i] = XMMatrixLookAtLH(eye, target, ups[i]);
    }

    faceProj = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.0f, nearZ, farZ);
}

void ShadowMap::UpdateShadowFrameCB()
{
    ShadowFrameCBData data;
    data.LightPosFarZ = XMFLOAT4(lightPos.x, lightPos.y, lightPos.z, farZ);
    game->Context->UpdateSubresource(shadowFrameCB.Get(), 0, nullptr, &data, 0, 0);
}

void ShadowMap::BeginShadowFace(int faceIdx)
{
    currentFace = faceIdx;

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    game->Context->PSSetShaderResources(1, 1, nullSRV);

    const float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    game->Context->ClearRenderTargetView(faceRTV[faceIdx].Get(), clearColor);
    game->Context->ClearDepthStencilView(depthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    game->Context->OMSetRenderTargets(1, faceRTV[faceIdx].GetAddressOf(), depthView.Get());
    game->Context->RSSetViewports(1, &viewport);
    game->Context->RSSetState(rastState.Get());

    game->Context->IASetInputLayout(Layout.Get());
    game->Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->Context->VSSetShader(shadowVS.Get(), nullptr, 0);
    game->Context->PSSetShader(shadowPS.Get(), nullptr, 0);

    UpdateShadowFrameCB();
    game->Context->PSSetConstantBuffers(1, 1, shadowFrameCB.GetAddressOf());
}

XMMATRIX ShadowMap::GetLightViewProj() const
{
    return faceView[currentFace] * faceProj;
}

void ShadowMap::DestroyResources()
{
    SRV.Reset();
    Sampler.Reset();
    Layout.Reset();
    cubeTexture.Reset();
    for (int i = 0; i < FACE_COUNT; ++i) faceRTV[i].Reset();
    depthTexture.Reset();
    depthView.Reset();
    shadowVS.Reset();
    shadowPS.Reset();
    rastState.Reset();
    shadowFrameCB.Reset();
}
