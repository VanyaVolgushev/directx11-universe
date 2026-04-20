cbuffer PerObject : register(b0)
{
    matrix World;
    matrix WVP;
    matrix LightWVP;
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
};

cbuffer ShadowFrame : register(b1)
{
    float4 LightPosFarZ;
};

struct VS_INPUT
{
    float3 Pos      : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos      : SV_POSITION;
    float3 WorldPos : POSITION1;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT o;
    float4 p = float4(input.Pos, 1.0f);
    o.WorldPos = mul(p, World).xyz;
    o.Pos      = mul(p, LightWVP);
    return o;
}

float PSMain(VS_OUTPUT input) : SV_TARGET
{
    float dist = length(input.WorldPos - LightPosFarZ.xyz);
    return saturate(dist / LightPosFarZ.w);
}
