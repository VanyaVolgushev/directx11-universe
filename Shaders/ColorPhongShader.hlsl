#include "PhongLighting.hlsli"

cbuffer PerObject : register(b0)
{
    matrix World;
    matrix WVP;
    matrix LightWVP;
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
};

struct VS_INPUT
{
    float3 Pos      : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
};

struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float3 WorldPos : POSITION1;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 p = float4(input.Pos, 1.0f);
    output.Pos      = mul(p, WVP);
    output.WorldPos = mul(p, World).xyz;
    output.Normal   = normalize(mul(input.Normal, (float3x3) World));
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float shadow = ComputeShadowFactor(input.WorldPos);
    return CalculatePhong(input.WorldPos, input.Normal, MaterialDiffuse,
                          MaterialAmbient, MaterialSpecular, shadow);
}
