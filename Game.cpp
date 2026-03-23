#include "Game.h"
#include <iostream>

extern InputDevice* g_InputDevice; // temporary bridge – better to remove later

Game::Game(int w, int h, const std::wstring& title)
    : ClientWidth(w), ClientHeight(h), Name(title)
{
    Display = std::make_unique<DisplayWin32>(w, h, title, this);
    InputDevice = std::make_unique<InputDevice>(Display->hWnd);
    g_InputDevice = InputDevice.get(); // temporary

    Components.push_back(std::make_unique<TriangleComponent>(this));
}

Game::~Game()
{
    g_InputDevice = nullptr;
}

void Game::Initialize()
{
    StartTime = std::chrono::steady_clock::now();
    PrevTime = StartTime;

    // Create D3D11 device & swap chain
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1 };

    DXGI_SWAP_CHAIN_DESC desc{};
    desc.BufferCount = 2;
    desc.BufferDesc.Width = ClientWidth;
    desc.BufferDesc.Height = ClientHeight;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = Display->hWnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        levels, 1, D3D11_SDK_VERSION,
        &desc, &SwapChain, &Device,
        nullptr, reinterpret_cast<ID3D11DeviceContext**>(&Context));

    if (FAILED(hr)) {
        std::cerr << "Cannot create D3D11 device/swapchain\n";
        Exit();
        return;
    }

    CreateBackBuffer();

    for (auto& comp : Components)
        comp->Initialize();
}

void Game::CreateBackBuffer()
{
    SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &RenderView);
}

void Game::Run()
{
    MSG msg{};

    while (!IsExiting)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) break;

        if (InputDevice->IsKeyDown(Keys::Escape)) {
            Exit();
            break;
        }

        auto now = std::chrono::steady_clock::now();
        float delta = std::chrono::duration<float>(now - PrevTime).count();
        PrevTime = now;
        TotalTime += delta;

        Update();
        PrepareFrame();
        Draw();
        EndFrame();
    }
}

void Game::Update()
{
    for (auto& comp : Components)
        comp->Update();
}

void Game::PrepareFrame()
{
    float color[] = { sinf(TotalTime) * 0.5f + 0.5f, 0.1f, 0.1f, 1 };
    Context->ClearRenderTargetView(RenderView.Get(), color);
}

void Game::Draw()
{
    Context->OMSetRenderTargets(1, &RenderView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(ClientWidth);
    vp.Height = static_cast<float>(ClientHeight);
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    Context->RSSetViewports(1, &vp);

    for (auto& comp : Components)
        comp->Draw();
}

void Game::EndFrame()
{
    SwapChain->Present(1, 0);
}

void Game::Exit()
{
    IsExiting = true;
}

void Game::RestoreTargets()
{
    Context->OMSetRenderTargets(1, &RenderView.GetAddressOf(), nullptr);
}