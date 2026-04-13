#pragma once
#include <DirectXMath.h>

class ITransformProvider
{
public:
    virtual ~ITransformProvider() = default;
    virtual DirectX::XMFLOAT3 GetPosition() const = 0;
    virtual DirectX::XMFLOAT3 GetRotation() const = 0;
};
