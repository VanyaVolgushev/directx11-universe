#include <iostream>
#include <random>

#include "ITransformProvider.h"
#include "Game.h"
#include "Helpers/LoadOBJ.h"
#include "GameComponent/GameComponents.h"
#include "GameComponent/PointLightComponent.h"

int main()
{
    Game game(L"PlanetOrbiter", 1024, 768);

    // --- CREATE EARTH AND SUN ---
    auto sun = new PlanetComponent(&game, nullptr, 0.0f, 0.0f, 3.0f, { 1.0f, 1.0f, 0.8f, 1.0f }, PlanetShape::Sphere, 0.2f, true, false);
    game.Components.push_back(sun);
    auto earth = new PlanetComponent(&game, sun, 14.0f, 1.0f, 1.0f, { 0.0f, 0.0f, 0.0f, 1.0f }, PlanetShape::Sphere, 2.0f, false);
    game.Components.push_back(earth);
    auto moon = new PlanetComponent(&game, earth, 2.0f, 4.0f, 0.3f, { 0.7f, 0.7f, 0.7f, 1.0f }, PlanetShape::Box, -4.0f, false);
    game.Components.push_back(moon);
    
    // --- CREATE MOON MESH ---    
    {
        std::vector<Vertex> fbxVertices;
        std::vector<int> fbxIndices;

        if (LoadOBJ("D:\\CG_labs\\lab2\\Lab2\\Assets\\cube.obj", fbxVertices, fbxIndices)) {
            MeshRenderer* fbxModel = new MeshRenderer(
                &game,
                moon,
                fbxVertices,
                fbxIndices,
                L"",
                L"./Shaders/ColorPhongShader.hlsl"
            );
            fbxModel->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
            fbxModel->SetScale(DirectX::XMFLOAT3(0.4f, 0.4f, 0.4f));
            // Set material
            MaterialData boxMat;
            boxMat.Ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
            boxMat.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
            boxMat.Specular = { 0.3f, 0.3f, 0.3f, 32.0f };
            fbxModel->SetMaterial(boxMat);
            game.Components.push_back(fbxModel);

        }
    }

    game.Components.push_back(new OrbitalCam(&game, 50.0f, earth)); // Also add a camera to earth

    // --- CREATE LIGHT ---
    auto sunLight = new PointLightComponent(&game, { 0, 0, 0 }, { 10.0f, 10.0f, 8.0f, 1.0f });
    game.Components.push_back(sunLight);

    // --- CREATE EARTH MESH---
    {
        std::vector<Vertex> fbxVertices;
        std::vector<int> fbxIndices;

        if (LoadOBJ("D:\\CG_labs\\lab2\\Lab2\\Assets\\earth.obj", fbxVertices, fbxIndices)) {
            MeshRenderer* fbxModel = new MeshRenderer(
                &game,
                earth,
                fbxVertices,
                fbxIndices,
                L"./Assets/EarthTexture.jpeg",
                L"./Shaders/TexturedPhongShader.hlsl"
            );
            fbxModel->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
            fbxModel->SetScale(DirectX::XMFLOAT3(1.1f, 1.1f, 1.1f));
            // Set material
            MaterialData earthMat;
            earthMat.Ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
            earthMat.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
            earthMat.Specular = { 0.3f, 0.3f, 0.3f, 32.0f };
            fbxModel->SetMaterial(earthMat);
            game.Components.push_back(fbxModel);
            
        }
    }

    // --- CREATE ROLLER ---
    RollingSphereComponent* roller = new RollingSphereComponent(&game, earth, 0.17f, 2.0f, 3.0f);
    game.Components.push_back(roller);

    // --- CREATE ROLLER'S MESH
    {
        std::vector<Vertex> fbxVertices;
        std::vector<int> fbxIndices;

        if (LoadOBJ("D:\\CG_labs\\lab2\\Lab2\\Assets\\gorbino.obj", fbxVertices, fbxIndices)) {
            MeshRenderer* fbxModel = new MeshRenderer(
                &game,
                roller,
                fbxVertices,
                fbxIndices,
                L"./Assets/gorbino.png",
                L"./Shaders/TexturedPhongShader.hlsl"
            );
            fbxModel->SetScale(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

            game.Components.push_back(fbxModel);
        }
    }
    // --- CREATE PICKUPS ---
    {
        // Statue of liberty
        {
            DirectX::XMFLOAT3 vectorFromCenterOfGlobePointingToStatue = { 0.860f, 0.525f, -0.267f };
            StickyCollectibleComponent* stickyPoint = new StickyCollectibleComponent(
                &game,                     // Game*
                earth,                // The PlanetComponent
                roller,          // The RollingSphereComponent 
                vectorFromCenterOfGlobePointingToStatue,     // Local direction on the planet surface
                0.045f,                     // Height offset (how far off the exact surface)
                0.17f             // Pass in your sphere's radius here for the trigger
            );
            game.Components.push_back(stickyPoint);

            // Create Mesh
            {
                std::vector<Vertex> fbxVertices;
                std::vector<int> fbxIndices;

                if (LoadOBJ("D:\\CG_labs\\lab2\\Lab2\\Assets\\StatueOfLiberty.obj", fbxVertices, fbxIndices)) {
                    MeshRenderer* fbxModel = new MeshRenderer(
                        &game,
                        stickyPoint,
                        fbxVertices,
                        fbxIndices,
                        L"./Assets/StatueOfLiberty.png",
                        L"./Shaders/TexturedPhongShader.hlsl"
                    );

                    fbxModel->SetScale(DirectX::XMFLOAT3(0.003f, 0.003f, 0.003f));
                    game.Components.push_back(fbxModel);
                }
            }
        }
        // Eiffel tower
        {
            DirectX::XMFLOAT3 vectorFromCenterOfGlobePointingToStatue = { 0.360f, 0.525f, -0.267f };
            StickyCollectibleComponent* stickyPoint = new StickyCollectibleComponent(
                &game,                     // Game*
                earth,                     // The PlanetComponent
                roller,                    // The RollingSphereComponent 
                vectorFromCenterOfGlobePointingToStatue, // Local direction on the planet surface
                0.045f,                    // Height offset
                0.17f                      // Trigger radius
            );
            game.Components.push_back(stickyPoint);

            // Create Mesh
            {
                std::vector<Vertex> fbxVertices;
                std::vector<int> fbxIndices;

                if (LoadOBJ("D:\\CG_labs\\lab2\\Lab2\\Assets\\EiffelTower.obj", fbxVertices, fbxIndices)) {
                    MeshRenderer* fbxModel = new MeshRenderer(
                        &game,
                        stickyPoint,
                        fbxVertices,
                        fbxIndices,
                        L"./Assets/EiffelTower.jpg",
                        L"./Shaders/TexturedPhongShader.hlsl"
                    );

                    fbxModel->SetScale(DirectX::XMFLOAT3(0.00001f, 0.00001f, 0.00001f));
                    fbxModel->SetRotation(DirectX::XMFLOAT3(DirectX::XM_PIDIV2, 0.0f, 0.0f));

                    game.Components.push_back(fbxModel);
                }
            }
        }
        // Pisa tower
        {
            DirectX::XMFLOAT3 vectorFromCenterOfGlobePointingToStatue = { 0.642154f, 0.355212f, 0.695714f };
            StickyCollectibleComponent* stickyPoint = new StickyCollectibleComponent(
                &game,                     // Game*
                earth,                     // The PlanetComponent
                roller,                    // The RollingSphereComponent 
                vectorFromCenterOfGlobePointingToStatue, // Local direction on the planet surface
                0.045f,                    // Height offset
                0.17f                      // Trigger radius
            );
            game.Components.push_back(stickyPoint);

            // Create Mesh
            {
                std::vector<Vertex> fbxVertices;
                std::vector<int> fbxIndices;

                if (LoadOBJ("D:\\CG_labs\\lab2\\Lab2\\Assets\\Pisa.obj", fbxVertices, fbxIndices)) {
                    MeshRenderer* fbxModel = new MeshRenderer(
                        &game,
                        stickyPoint,
                        fbxVertices,
                        fbxIndices,
                        L"./Assets/Pisa.jpg",
                        L"./Shaders/TexturedPhongShader.hlsl"
                    );

                    fbxModel->SetScale(DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));
                    fbxModel->SetRotation(DirectX::XMFLOAT3(DirectX::XM_PIDIV2, 0.0f, 0.0f));

                    game.Components.push_back(fbxModel);
                }
            }
        }
    }
    // --- CREATE OTHER ASTRAL BODIES ---
    {
        auto mercury = new PlanetComponent(&game, sun, 6.0f, 2.5f, 0.4f, { 0.6f, 0.6f, 0.6f, 1.0f }, PlanetShape::Sphere, 0.1f);
        game.Components.push_back(mercury);

        auto venus = new PlanetComponent(&game, sun, 9.0f, 1.8f, 0.8f, { 0.9f, 0.5f, 0.2f, 1.0f }, PlanetShape::Sphere, -0.1f);
        game.Components.push_back(venus);

        auto mars = new PlanetComponent(&game, sun, 19.0f, 0.8f, 0.7f, { 0.8f, 0.3f, 0.1f, 1.0f }, PlanetShape::Sphere, 1.9f);
        game.Components.push_back(mars);
        auto phobos = new PlanetComponent(&game, mars, 1.5f, 5.0f, 0.2f, { 0.5f, 0.4f, 0.4f, 1.0f }, PlanetShape::Box, 3.0f);
        game.Components.push_back(phobos);

        // --- CREATE ASTEROID BELT ---
        {
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

                PlanetShape shape = (i % 2 == 0) ? PlanetShape::Box : PlanetShape::Sphere;

                auto asteroid = new PlanetComponent(&game, sun, r, speed, size, { gray, gray, gray, 1.0f }, shape, rot);
                game.Components.push_back(asteroid);
            }
        }

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
        game.Components.push_back(new CameraSwitcher(&game));
    }

    game.Initialize();
    game.Run();
    std::cout << "Goodbye World!\n";
    return 0;
}
