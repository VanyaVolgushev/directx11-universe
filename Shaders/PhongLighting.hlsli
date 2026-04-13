// Shared Per-Frame data
cbuffer PerFrame : register(b1)
{
    float4 LightPos;
    float4 LightColor;
    float4 CameraPos;
    float4 LightParams; // x: AmbientIntensity, y: Constant, z: Linear, w: Quadratic
};

// The core lighting function
float4 CalculatePhong(float3 worldPos, float3 normal, float4 albedo, float4 ambientMat, float4 specMat)
{
    float3 N = normalize(normal);
    float3 L = normalize(LightPos.xyz - worldPos);
    float3 V = normalize(CameraPos.xyz - worldPos);
    float3 R = reflect(-L, N);

    // Attenuation
    float dist = distance(LightPos.xyz, worldPos);
    float attenuation = 1.0f / (LightParams.y + (LightParams.z * dist) + (LightParams.w * dist * dist));

    // Ambient
    float4 ambient = ambientMat * LightParams.x;

    // Diffuse
    float diffIntensity = max(dot(N, L), 0.0f);
    float4 diffuse = diffIntensity * albedo * LightColor;

    // Specular (using specMat.w as shininess)
    float specIntensity = pow(max(dot(R, V), 0.0f), max(specMat.w, 1.0f));
    float4 specular = specIntensity * float4(specMat.xyz, 1.0f) * LightColor;

    float4 finalColor = ambient + (diffuse + specular) * attenuation;
    finalColor.a = albedo.a;
    
    return saturate(finalColor);
}