#pragma once
#include <vector>
#include <directxmath.h>
#include "Vertex.h"

static void GenerateSphere(float radius, int latLines, int longLines, std::vector<Vertex>& outVertices, std::vector<int>& outIndices) {
    using namespace DirectX;

    for (int i = 0; i <= latLines; i++) {
        float phi = i * XM_PI / latLines;
        for (int j = 0; j <= longLines; j++) {
            float theta = j * 2.0f * XM_PI / longLines;

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            Vertex v;
            v.Position = { x * radius, y * radius, z * radius };
            v.Normal = { x, y, z };
            v.TexCoord = { (float)j / longLines, (float)i / latLines };
            outVertices.push_back(v);
        }
    }

    for (int i = 0; i < latLines; i++) {
        for (int j = 0; j < longLines; j++) {
            int first = (i * (longLines + 1)) + j;
            int second = first + longLines + 1;

            outIndices.push_back(first);
            outIndices.push_back(first + 1);
            outIndices.push_back(second);

            outIndices.push_back(second);
            outIndices.push_back(first + 1);
            outIndices.push_back(second + 1);
        }
    }
}

static void GenerateBox(float size, std::vector<Vertex>& outVertices, std::vector<int>& outIndices) {
    // Helper to generate a box with proper normals per face
    struct BoxVertex { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT3 norm; };
    float s = size;

    // Positions and Normals for 6 faces (24 vertices)
    DirectX::XMFLOAT3 pos[] = {
        {-s,-s,-s}, {-s,s,-s}, {s,s,-s}, {s,-s,-s}, // Front
        {-s,-s,s}, {-s,s,s}, {s,s,s}, {s,-s,s},     // Back
        {-s,s,-s}, {-s,s,s}, {s,s,s}, {s,s,-s},     // Top
        {-s,-s,-s}, {-s,-s,s}, {s,-s,s}, {s,-s,-s}, // Bottom
        {-s,-s,-s}, {-s,-s,s}, {-s,s,s}, {-s,s,-s}, // Left
        {s,-s,-s}, {s,-s,s}, {s,s,s}, {s,s,-s}      // Right
    };
    DirectX::XMFLOAT3 normals[] = {
        {0,0,-1}, {0,0,-1}, {0,0,-1}, {0,0,-1},
        {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1},
        {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0},
        {0,-1,0}, {0,-1,0}, {0,-1,0}, {0,-1,0},
        {-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0},
        {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}
    };

    for (int i = 0; i < 24; ++i) {
        Vertex v;
        v.Position = pos[i];
        v.Normal = normals[i];
        v.TexCoord = { 0,0 }; // Simple wrap
        outVertices.push_back(v);
    }

    int baseIndices[] = { 0,1,2, 0,2,3 };
    for (int f = 0; f < 6; ++f) {
        for (int i = 0; i < 6; ++i) outIndices.push_back(baseIndices[i] + (f * 4));
    }
}
