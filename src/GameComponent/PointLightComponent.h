#pragma once
#include "GameComponent.h"
#include "../LightData.h"

class PointLightComponent : public GameComponent {
public:
    PointLightComponent(Game* game, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color)
        : GameComponent(game) {
        Data.Position = { pos.x, pos.y, pos.z, 1.0f };
        Data.Color = color;
        Data.AmbientIntensity = 0.0f;
        Data.ConstantAttenuation = 1.0f;
        Data.LinearAttenuation = 0.00f;
        Data.QuadraticAttenuation = 0.01f;
    }

    float ShadowNearZ   = 5.0f;
    float ShadowFarZ    = 200.0f;
    float ShadowBias    = 0.002f;
    float ShadowAmbient = 0.35f;

    PointLightData Data;
};
