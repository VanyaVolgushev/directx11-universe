#pragma once
#include <windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <chrono>

class DisplayWin32;
class InputDevice;
class GameComponent;
class CameraComponent;

class Game
{
public:
    const float BG_COLOR[3] = {0.0f, 0.0f, 0.0f};

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
    Microsoft::WRL::ComPtr<ID3D11Device> Device;
    HINSTANCE Instance;
    LPCWSTR Name;
    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderSRV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderView;
    bool ScreenResized = false;
    std::chrono::time_point<std::chrono::steady_clock> StartTime;
    Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;

    DisplayWin32* Display = nullptr;
    InputDevice* InputDevice = nullptr;
    CameraComponent* MainCamera = nullptr;
    std::vector<GameComponent*> Components;

    bool isExitRequested = false;

    Game(LPCWSTR name, int screenWidth, int screenHeight);
    virtual ~Game();

    void Initialize();
    void Run();
    void Exit();
    void Update();
    void Draw();
    void EndFrame();
    void PrepareFrame();
    void PrepareResources();
    void RestoreTargets();
    void UpdateInternal();
    LRESULT MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

private:
    void CreateBuffers();
    void DestroyResources();
};