#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <chrono>
#include <string>
#include <vector>
#include <wrl/client.h>

#include "DisplayWin32.h"
#include "InputDevice.h"
#include "GameComponent.h"
#include "TriangleComponent.h"

using Microsoft::WRL::ComPtr;

class Game
{
public:
    ComPtr<ID3D11Device>           Device;
    ID3D11DeviceContext* Context = nullptr;     // non-owning raw pointer (common pattern)
    ComPtr<IDXGISwapChain>         SwapChain;
    ComPtr<ID3D11Texture2D>        backBuffer;
    ComPtr<ID3D11RenderTargetView> RenderView;
    // ComPtr<ID3D11ShaderResourceView> RenderSRV;     // optional

    std::unique_ptr<DisplayWin32>  Display;
    std::unique_ptr<InputDevice>   InputDevice;

    std::vector<std::unique_ptr<GameComponent>> Components;

    std::wstring                   Name;
    std::chrono::steady_clock::time_point StartTime;
    std::chrono::steady_clock::time_point PrevTime;
    float                          TotalTime = 0;
    bool                           IsExiting = false;

    int                            ClientWidth;
    int                            ClientHeight;

public:
    Game(int width, int height, const std::wstring& title);
    ~Game();

    void Initialize();
    void Run();
    void Update();
    void Draw();
    void EndFrame();
    void Exit();

    void PrepareResources();
    void PrepareFrame();
    void RestoreTargets();

private:
    void CreateBackBuffer();
    void MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};