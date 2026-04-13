cbuffer PerObject : register(b0)
{
    matrix World;
    matrix WVP;
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular; // w = shininess
};

cbuffer PerFrame : register(b1)
{
    float4 LightPos;
    float4 LightColor;
    float4 CameraPos;
    float4 LightParams; // x: ambient, y: const, z: linear, w: quad
};

Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

struct VS_IN
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 Normal : NORMAL;
};

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float3 Normal : NORMAL;
    float3 WorldPos : POSITION1;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.WorldPos = mul(float4(input.Pos, 1.0f), World).xyz;
    output.Normal = normalize(mul(input.Normal, (float3x3) World));
    output.Tex = input.Tex;
    return output;
}

float4 PSMain(PS_IN input) : SV_TARGET
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(LightPos.xyz - input.WorldPos);
    float3 V = normalize(CameraPos.xyz - input.WorldPos);
    float3 R = reflect(-L, N);

    float dist = distance(LightPos.xyz, input.WorldPos);
    float atten = 1.0f / (LightParams.y + LightParams.z * dist + LightParams.w * dist * dist);

    // Ambient
    float4 ambient = MaterialAmbient * LightParams.x;

    // Diffuse
    float diff = max(dot(N, L), 0.0f);
    float4 diffuse = diff * MaterialDiffuse * LightColor;

    // Specular
    float spec = pow(max(dot(R, V), 0.0f), MaterialSpecular.w);
    float4 specular = spec * MaterialSpecular * LightColor;

    float4 texColor = objTexture.Sample(objSampler, input.Tex);
    
    // Final combine
    float4 lighting = ambient + (diffuse + specular) * atten;
    return texColor * lighting;
}