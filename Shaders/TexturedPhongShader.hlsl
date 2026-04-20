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

Texture2D    objTexture : register(t0);
SamplerState objSampler : register(s0);

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
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
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
    float4 texColor = objTexture.Sample(objSampler, input.TexCoord);

    // Mix the texture with the material diffuse color
    float4 albedo = texColor * MaterialDiffuse;

    float shadow = ComputeShadowFactor(input.WorldPos);
    return CalculatePhong(input.WorldPos, input.Normal, albedo,
                          MaterialAmbient, MaterialSpecular, shadow);
}
