#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

#include "Vertex.h"

static bool LoadOBJ(const std::string& filePath, std::vector<Vertex>& outVertices, std::vector<int>& outIndices) {
    Assimp::Importer importer;
    // Read file and apply necessary DirectX conversions
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_FlipUVs);

    std::string error = importer.GetErrorString();
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    // Keep track of how many vertices we've added so far
    // so we can offset the indices of subsequent meshes properly
    unsigned int vertexOffset = 0;

    // Loop through ALL meshes in the file
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        // Extract Vertices for this mesh
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            if (mesh->mTextureCoords[0]) { // Does it have texture coordinates?
                vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            }
            else {
                vertex.TexCoord = { 0.0f, 0.0f };
            }

            if (mesh->HasNormals()) {
                vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            }

            outVertices.push_back(vertex);
        }

        // Extract Indices for this mesh
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                // Add the vertexOffset so the indices point to the correct vertices in the combined list
                outIndices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        // Increment the offset by the number of vertices we just added
        vertexOffset += mesh->mNumVertices;
    }

    return true;
}