cbuffer ConstantBuffer : register(b0)
{
    matrix WVP;
};

// Texture and Sampler
Texture2D objTexture : register(t0);
SamplerState objSamplerState : register(s0);

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // Sample the color from the texture using the UV coordinates
    return objTexture.Sample(objSamplerState, input.TexCoord);
}