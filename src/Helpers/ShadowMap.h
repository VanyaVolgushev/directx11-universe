#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

class Game;

// Omnidirectional cube shadow map for a single point light.
class ShadowMap
{
public:
    static constexpr UINT SHADOW_MAP_SIZE = 4096;
    static constexpr int  FACE_COUNT      = 6;

    explicit ShadowMap(Game* game);

    void Initialize();
    void DestroyResources();

    void UpdateLightMatrices(const DirectX::XMFLOAT3& lightPos, float nearZ, float farZ);
    void BeginShadowFace(int faceIdx);

    DirectX::XMMATRIX GetLightViewProj() const;
    float GetFarZ() const { return farZ; }

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       Sampler;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        Layout;

private:
    void UpdateShadowFrameCB();

    Game* game;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>        cubeTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> faceRTV[FACE_COUNT];
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        depthTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthView;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>     shadowVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>      shadowPS;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>  rastState;
    Microsoft::WRL::ComPtr<ID3D11Buffer>           shadowFrameCB;

    D3D11_VIEWPORT viewport;

    DirectX::XMMATRIX faceView[FACE_COUNT];
    DirectX::XMMATRIX faceProj;
    DirectX::XMFLOAT3 lightPos;
    float nearZ = 1.0f;
    float farZ  = 100.0f;
    int   currentFace = 0;
};
