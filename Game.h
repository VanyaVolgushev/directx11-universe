#pragma once
#include <windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <chrono>

class DisplayWin32;
class InputDevice;
class GameComponent;

class Game
{
public:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
    void* DebugAnnotation = nullptr; // Reserved/Placeholder
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

    DisplayWin32* Display = nullptr;
    InputDevice* InputDevice = nullptr;
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
    void CreateBackBuffer();
    void DestroyResources();
};