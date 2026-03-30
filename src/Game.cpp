#include "Game.h"
#include "DisplayWin32.h"
#include "InputDevice.h"
#include "GameComponent.h"
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    Game* game = reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (game) return game->MessageHandler(hwnd, umessage, wparam, lparam);
    return DefWindowProc(hwnd, umessage, wparam, lparam);
}

Game::Game(LPCWSTR name, int screenWidth, int screenHeight) : Name(name)
{
    Instance = GetModuleHandle(nullptr);
    Display = new DisplayWin32(Name, Instance, screenWidth, screenHeight);
    // Bind HWND to this Game instance for the global message handler
    SetWindowLongPtr(Display->hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    StartTime = std::chrono::steady_clock::now();
    PrevTime = StartTime;
}

Game::~Game()
{
    DestroyResources();
    delete InputDevice;
    delete Display;
    for (auto component : Components) {
        delete component;
    }
}

void Game::Initialize()
{
    PrepareResources();
    CreateBuffers();

    InputDevice = new class InputDevice(this);

    for (auto component : Components) {
        component->Initialize();
    }
}

void Game::PrepareResources()
{
    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = Display->ClientWidth;
    swapDesc.BufferDesc.Height = Display->ClientHeight;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = Display->hWnd;
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_DEBUG, featureLevel, 1, D3D11_SDK_VERSION,
        &swapDesc, &SwapChain, &Device, nullptr, &Context);
}

// Inside Game.cpp
void Game::CreateBuffers()
{
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
    Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, &RenderView);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = Display->ClientWidth;
    depthDesc.Height = Display->ClientHeight;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    Device->CreateTexture2D(&depthDesc, nullptr, &depthStencil);
    Device->CreateDepthStencilView(depthStencil.Get(), nullptr, &DepthView);
}

void Game::RestoreTargets()
{
    // --- UPDATED: Bind and clear DepthView alongside RenderView ---
    Context->OMSetRenderTargets(1, RenderView.GetAddressOf(), DepthView.Get());
    float color[] = { 0.05f, 0.05f, 0.1f, 1.0f }; // Dark space background
    Context->ClearRenderTargetView(RenderView.Get(), color);
    Context->ClearDepthStencilView(DepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Game::DestroyResources()
{
    for (auto component : Components) {
        component->DestroyResources();
    }
    RenderView.Reset();
    RenderSRV.Reset();
    BackBuffer.Reset();
    SwapChain.Reset();
    Context.Reset();
    Device.Reset();
}

void Game::Run()
{
    MSG msg = {};
    while (!isExitRequested)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) {
            isExitRequested = true;
        }

        UpdateInternal();
        Draw();
        EndFrame();
    }
}

void Game::Exit()
{
    isExitRequested = true;
}

void Game::UpdateInternal()
{
    auto curTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
    PrevTime = curTime;
    TotalTime += deltaTime;

    Update();
}

void Game::Update()
{
    if (InputDevice && InputDevice->IsKeyDown(Keys::Escape)) {
        Exit();
    }

    for (auto component : Components) {
        component->Update();
    }
}

void Game::PrepareFrame()
{
    Context->ClearState();

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display->ClientWidth);
    viewport.Height = static_cast<float>(Display->ClientHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    Context->RSSetViewports(1, &viewport);
}

void Game::Draw()
{
    PrepareFrame();
    RestoreTargets();

    for (auto component : Components) {
        component->Draw();
    }
}

void Game::EndFrame()
{
    Context->OMSetRenderTargets(0, nullptr, nullptr);
    SwapChain->Present(1, 0);
}

LRESULT Game::MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_INPUT:
    {
        if (!InputDevice) return DefWindowProc(hwnd, umessage, wparam, lparam);

        UINT dwSize = 0;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == nullptr) return 0;

        if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
            OutputDebugString(TEXT("GetRawInputData size mismatch\n"));

        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);
        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            InputDevice->OnKeyDown({ raw->data.keyboard.MakeCode, raw->data.keyboard.Flags, raw->data.keyboard.VKey, raw->data.keyboard.Message });
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE) {
            InputDevice->OnMouseMove({ raw->data.mouse.usFlags, raw->data.mouse.usButtonFlags,
                static_cast<int>(raw->data.mouse.ulExtraInformation), static_cast<int>(raw->data.mouse.ulRawButtons),
                static_cast<int>(raw->data.mouse.usButtonData), raw->data.mouse.lLastX, raw->data.mouse.lLastY });
        }

        delete[] lpb;
        return DefWindowProc(hwnd, umessage, wparam, lparam);
    }
    default:
        return DefWindowProc(hwnd, umessage, wparam, lparam);
    }
}