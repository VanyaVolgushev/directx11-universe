#include <iostream>
#include <random>

#include "PlanetComponent.h"
#include "Game.h"
#include "FPSCam.h"
#include "OrbitalCam.h"
#include "IsometricCam.h"
#include "CameraSwitcher.h"
#include "MeshRenderer.h"
#include "Helpers/LoadFBX.h"

int main()
{
    Game game(L"PlanetOrbiter", 1024, 768);

    auto sun = new PlanetComponent(&game, nullptr, 0.0f, 0.0f, 3.0f, { 1.0f, 0.9f, 0.0f, 1.0f }, PlanetShape::Sphere, 0.2f);
    game.Components.push_back(sun);

    auto mercury = new PlanetComponent(&game, sun, 6.0f, 2.5f, 0.4f, { 0.6f, 0.6f, 0.6f, 1.0f }, PlanetShape::Sphere, 0.1f);
    game.Components.push_back(mercury);

    auto venus = new PlanetComponent(&game, sun, 9.0f, 1.8f, 0.8f, { 0.9f, 0.5f, 0.2f, 1.0f }, PlanetShape::Sphere, -0.1f);
    game.Components.push_back(venus);

    auto earth = new PlanetComponent(&game, sun, 14.0f, 1.0f, 1.0f, { 1.0f, 1.0f, 0.9f, 1.0f }, PlanetShape::Sphere, 2.0f);
    game.Components.push_back(earth);
    auto moon = new PlanetComponent(&game, earth, 2.0f, 4.0f, 0.3f, { 0.7f, 0.7f, 0.7f, 1.0f }, PlanetShape::Box, -4.0f);
    game.Components.push_back(moon);
    game.Components.push_back(new OrbitalCam(&game, 50.0f, earth)); // Also add a camera to earth

    auto mars = new PlanetComponent(&game, sun, 19.0f, 0.8f, 0.7f, { 0.8f, 0.3f, 0.1f, 1.0f }, PlanetShape::Sphere, 1.9f);
    game.Components.push_back(mars);
    auto phobos = new PlanetComponent(&game, mars, 1.5f, 5.0f, 0.2f, { 0.5f, 0.4f, 0.4f, 1.0f }, PlanetShape::Box, 3.0f);
    game.Components.push_back(phobos);

    // --- ASTEROID BELT ---
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> beltRadiusDist(21.0f, 26.0f); // Between Mars (19) and Jupiter (28)
    std::uniform_real_distribution<float> speedDist(0.45f, 0.65f);      // Orbital speeds
    std::uniform_real_distribution<float> sizeDist(0.05f, 0.15f);       // Very small sizes
    std::uniform_real_distribution<float> colorDist(0.4f, 0.6f);        // Grayscale rock colors
    std::uniform_real_distribution<float> rotDist(-5.0f, 5.0f);         // Random rotation speeds for asteroids

    const int NUM_ASTEROIDS = 200;
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        float r = beltRadiusDist(gen);
        float speed = speedDist(gen);
        float size = sizeDist(gen);
        float gray = colorDist(gen);
        float rot = rotDist(gen);

        // 50/50 mix of boxes and spheres
        PlanetShape shape = (i % 2 == 0) ? PlanetShape::Box : PlanetShape::Sphere;

        auto asteroid = new PlanetComponent(&game, sun, r, speed, size, { gray, gray, gray, 1.0f }, shape, rot);
        game.Components.push_back(asteroid);
    }
    // --- END ASTEROID BELT ---

    auto jupiter = new PlanetComponent(&game, sun, 28.0f, 0.4f, 2.0f, { 0.8f, 0.7f, 0.6f, 1.0f }, PlanetShape::Sphere, 4.8f);
    game.Components.push_back(jupiter);
    auto io = new PlanetComponent(&game, jupiter, 2.5f, 3.5f, 0.25f, { 0.9f, 0.9f, 0.2f, 1.0f }, PlanetShape::Sphere, 2.0f);
    game.Components.push_back(io);
    auto europa = new PlanetComponent(&game, jupiter, 3.2f, 2.8f, 0.2f, { 0.8f, 0.8f, 0.9f, 1.0f }, PlanetShape::Sphere, 1.8f);
    game.Components.push_back(europa);
    auto ganymede = new PlanetComponent(&game, jupiter, 4.0f, 2.0f, 0.35f, { 0.5f, 0.5f, 0.5f, 1.0f }, PlanetShape::Sphere, 1.2f);
    game.Components.push_back(ganymede);
    auto callisto = new PlanetComponent(&game, jupiter, 4.8f, 1.5f, 0.3f, { 0.4f, 0.4f, 0.4f, 1.0f }, PlanetShape::Sphere, 0.9f);
    game.Components.push_back(callisto);

    auto saturn = new PlanetComponent(&game, sun, 38.0f, 0.25f, 1.7f, { 0.9f, 0.8f, 0.5f, 1.0f }, PlanetShape::Sphere, 4.5f);
    game.Components.push_back(saturn);
    auto titan = new PlanetComponent(&game, saturn, 3.0f, 2.0f, 0.4f, { 0.8f, 0.6f, 0.2f, 1.0f }, PlanetShape::Sphere, 1.5f);
    game.Components.push_back(titan);
    auto enceladus = new PlanetComponent(&game, saturn, 2.0f, 3.5f, 0.15f, { 0.9f, 0.9f, 0.9f, 1.0f }, PlanetShape::Box, 3.0f);
    game.Components.push_back(enceladus);

    auto uranus = new PlanetComponent(&game, sun, 47.0f, 0.15f, 1.2f, { 0.6f, 0.8f, 0.9f, 1.0f }, PlanetShape::Sphere, -3.0f);
    game.Components.push_back(uranus);
    auto titania = new PlanetComponent(&game, uranus, 2.0f, 1.5f, 0.2f, { 0.7f, 0.7f, 0.7f, 1.0f }, PlanetShape::Sphere, 1.4f);
    game.Components.push_back(titania);

    auto neptune = new PlanetComponent(&game, sun, 55.0f, 0.1f, 1.1f, { 0.2f, 0.3f, 0.8f, 1.0f }, PlanetShape::Sphere, 3.2f);
    game.Components.push_back(neptune);
    auto triton = new PlanetComponent(&game, neptune, 2.0f, -1.8f, 0.25f, { 0.8f, 0.8f, 0.8f, 1.0f }, PlanetShape::Sphere, -1.5f);
    game.Components.push_back(triton);

    auto pluto = new PlanetComponent(&game, sun, 63.0f, 0.07f, 0.3f, { 0.8f, 0.7f, 0.6f, 1.0f }, PlanetShape::Sphere, 0.2f);
    game.Components.push_back(pluto);
    auto charon = new PlanetComponent(&game, pluto, 0.8f, 2.5f, 0.15f, { 0.6f, 0.6f, 0.6f, 1.0f }, PlanetShape::Box, 0.2f);
    game.Components.push_back(charon);

    game.Components.push_back(new IsometricCam(&game, 90.0f));
    game.Components.push_back(new FPSCam(&game));
    game.Components.push_back(new OrbitalCam(&game, 80.0f));
    game.Components.push_back(new OrbitalCam(&game, 80.0f));

    game.Components.push_back(new CameraSwitcher(&game));

    // --- CREATE EARTH MESH---
    {
        std::vector<Vertex> fbxVertices;
        std::vector<int> fbxIndices;

        if (LoadFBX("D:\\CG_labs\\lab2\\Lab2\\Assets\\earth.obj", fbxVertices, fbxIndices)) {
            MeshRenderer* fbxModel = new MeshRenderer(
                &game,
                earth,
                fbxVertices,
                fbxIndices,
                L"./Assets/EarthTexture.jpeg",
                L"./Shaders/TexturedShader.hlsl"
            );
            fbxModel->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
            fbxModel->SetScale(DirectX::XMFLOAT3(1.1f, 1.1f, 1.1f));

            game.Components.push_back(fbxModel);
        }
    }

    // --- CREATE EIFFEL TOWER MESH---
    {
        std::vector<Vertex> fbxVertices;
        std::vector<int> fbxIndices;

        if (LoadFBX("D:\\CG_labs\\lab2\\Lab2\\Assets\\StatueOfLiberty.obj", fbxVertices, fbxIndices)) {
            MeshRenderer* fbxModel = new MeshRenderer(
                &game,
                earth,
                fbxVertices,
                fbxIndices,
                L"./Assets/StatueOfLiberty.png",
                L"./Shaders/TexturedShader.hlsl"
            );
            using namespace DirectX;
            fbxModel->SetPosition(DirectX::XMFLOAT3(0.890f, 0.525f, -0.267f));
            fbxModel->SetScale(DirectX::XMFLOAT3(0.001f, 0.001f, 0.001f));
            fbxModel->SetRotation(DirectX::XMFLOAT3(0.0f + 1.57f, 0.92f + 2.0f, 3));

            game.Components.push_back(fbxModel);
        }
    }

    game.Initialize();
    game.Run();
    std::cout << "Goodbye World!\n";
    return 0;
}
