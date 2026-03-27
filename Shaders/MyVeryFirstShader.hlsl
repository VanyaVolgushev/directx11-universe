cbuffer ConstantBuffer : register(b0)
{
    matrix WVP; // World * View * Projection
    float4 PlanetColor;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(input.Pos, WVP);
    output.Color = PlanetColor;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return input.Color;
}