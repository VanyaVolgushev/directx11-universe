#pragma once

#pragma once
#include <directxmath.h>

struct PointLightData {
    DirectX::XMFLOAT4 Position; // World space
    DirectX::XMFLOAT4 Color;
    float AmbientIntensity;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
};

struct MaterialData {
    DirectX::XMFLOAT4 Ambient;
    DirectX::XMFLOAT4 Diffuse;
    DirectX::XMFLOAT4 Specular; // w is shininess
};