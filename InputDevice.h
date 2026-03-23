#pragma once
#include <unordered_set>
#include <windows.h>
#include <directxmath.h>
#include "Keys.h"
#include "Delegates.h"

class Game; // Forward declaration

class InputDevice
{
private:
    Game* game;
    std::unordered_set<Keys>* keys;

public:
    struct MouseMoveEventArgs
    {
        DirectX::XMFLOAT2 Position;
        DirectX::XMFLOAT2 Offset;
        int WheelDelta;
    };

    DirectX::XMFLOAT2 MousePosition;
    DirectX::XMFLOAT2 MouseOffset;
    int MouseWheelDelta;

    MulticastDelegate<const MouseMoveEventArgs&> MouseMove;

    InputDevice(Game* inGame);
    ~InputDevice();

    void AddPressedKey(Keys key);
    void RemovePressedKey(Keys key);
    bool IsKeyDown(Keys key);

protected:
    struct KeyboardInputEventArgs {
        USHORT MakeCode;
        USHORT Flags;
        USHORT VKey;
        UINT   Message;
    };

    enum class MouseButtonFlags {
        LeftButtonDown = 1, LeftButtonUp = 2,
        RightButtonDown = 4, RightButtonUp = 8,
        MiddleButtonDown = 16, MiddleButtonUp = 32,
        Button4Down = 64, Button4Up = 128,
        Button5Down = 256, Button5Up = 512,
        MouseWheel = 1024, Hwheel = 2048, None = 0,
    };

    struct RawMouseEventArgs {
        int Mode;
        int ButtonFlags;
        int ExtraInformation;
        int Buttons;
        int WheelDelta;
        int X;
        int Y;
    };

public: // Made accessible to Game's MessageHandler
    void OnKeyDown(KeyboardInputEventArgs args);
    void OnMouseMove(RawMouseEventArgs args);
};