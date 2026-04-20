cbuffer PerFrame : register(b1)
{
    float4 LightPos;
    float4 LightColor;
    float4 CameraPos;
    float4 LightParams;
    float4 ShadowParams;
    float4 LightDirection;
};

TextureCube<float> ShadowCube    : register(t1);
SamplerState       ShadowSampler : register(s1);

float ComputeShadowFactor(float3 worldPos)
{
    float3 lightToFrag = worldPos - LightPos.xyz;
    float dist = length(lightToFrag);

    float farZ = max(ShadowParams.y, 0.0001f);
    float normDist = dist / farZ;

    float bias = max(ShadowParams.z, 0.0005f);

    float3 dir = normalize(lightToFrag);
    float stored = ShadowCube.SampleLevel(ShadowSampler, dir, 0).r;

    return (normDist - bias < stored) ? 1.0f : 0.0f;
}

float4 CalculatePhong(float3 worldPos, float3 normal, float4 albedo,
                     float4 ambientMat, float4 specMat, float shadowFactor)
{
    float3 N = normalize(normal);
    float3 L = normalize(LightPos.xyz - worldPos);
    float3 V = normalize(CameraPos.xyz - worldPos);
    float3 R = reflect(-L, N);

    float dist = distance(LightPos.xyz, worldPos);
    float attenuation = 1.0f / (LightParams.y + (LightParams.z * dist) + (LightParams.w * dist * dist));

    float shadowAmbient = ShadowParams.x * (1.0f - shadowFactor);
    float4 ambient = ambientMat * (LightParams.x + shadowAmbient);

    float diffIntensity = max(dot(N, L), 0.0f);
    float4 diffuse = diffIntensity * albedo * LightColor;

    float specIntensity = pow(max(dot(R, V), 0.0f), max(specMat.w, 1.0f));
    float4 specular = specIntensity * float4(specMat.xyz, 1.0f) * LightColor;

    float4 lit = (diffuse + specular) * attenuation * shadowFactor;
    float4 finalColor = ambient + lit;
    finalColor.a = albedo.a;

    return saturate(finalColor);
}
