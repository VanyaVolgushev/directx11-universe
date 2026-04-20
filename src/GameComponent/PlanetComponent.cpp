#include "PlanetComponent.h"
#include "../Game.h"
#include "MeshRenderer.h"
#include "../Helpers/GenerateSphere.h"

#define ORBIT_DISTANCE_MULT 1.3f
#define ANGULAR_SPEED_MULT 0.4f

PlanetComponent::PlanetComponent(Game* game, PlanetComponent* parent, float orbitRadius, float angularSpeed, float radius, DirectX::XMFLOAT4 color, PlanetShape shape, float rotationSpeed, bool instantiateMesh, bool usePhongShader)
    : GameComponent(game), parent(parent), orbitRadius(orbitRadius* ORBIT_DISTANCE_MULT),
    angularSpeed(angularSpeed* ANGULAR_SPEED_MULT), radius(radius), color(color), shape(shape),
    currentPosition(0, 0, 0), currentRotation(0, 0, 0), rotationSpeed(rotationSpeed* ANGULAR_SPEED_MULT)
{
    if (instantiateMesh) {
    CreateMeshChild(shape, radius, game, color, usePhongShader);
    }
}

void PlanetComponent::CreateMeshChild(PlanetShape shape, float radius, Game* game, DirectX::XMFLOAT4& color, bool usePhongShader)
{
    std::vector<Vertex> verts;
    std::vector<int> inds;

    if (shape == PlanetShape::Sphere) {
        GenerateSphere(radius, 24, 24, verts, inds);
    }
    else {
        GenerateBox(radius, verts, inds);
    }

    auto shaderPath = L"./Shaders/ColorPhongShader.hlsl";
    if (!usePhongShader) {
        shaderPath = L"./Shaders/ColorShader.hlsl";
    }
    // Create the MeshRenderer child
    // We pass L"" for texture since we are using a Color Phong Shader
    MeshRenderer* renderer = new MeshRenderer(
        game,
        this,              // The planet is the parent transform provider
        verts,
        inds,
        L"",               // No texture
        shaderPath
    );

    // Setup Material based on Planet Color
    MaterialData mat;
    mat.Ambient = { color.x * 0.1f, color.y * 0.1f, color.z * 0.1f, 1.0f };
    mat.Diffuse = color;
    mat.Specular = { 0.5f, 0.5f, 0.5f, 16.0f };
    renderer->SetMaterial(mat);

    // Ensure the child mesh is at 0,0,0 relative to the planet
    renderer->SetPosition({ 0, 0, 0 });

    // Add to game components so it gets initialized and drawn
    game->Components.push_back(renderer);
}

void PlanetComponent::Update() {
    using namespace DirectX;

    float angle = angularSpeed * game->TotalTime;
    float localX = orbitRadius * cos(angle);
    float localZ = orbitRadius * sin(angle);

    currentPosition = { localX, 0.0f, localZ };

    if (parent) {
        XMFLOAT3 pPos = parent->GetPosition();
        currentPosition.x += pPos.x;
        currentPosition.y += pPos.y;
        currentPosition.z += pPos.z;
    }

    currentRotation.y = rotationSpeed * game->TotalTime;
}