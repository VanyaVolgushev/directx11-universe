#include "StickyCollectibleComponent.h"
#include "PlanetComponent.h"
#include "RollingSphereComponent.h"
#include "../Game.h"
#include <cmath>

StickyCollectibleComponent::StickyCollectibleComponent(Game* game,
    PlanetComponent* planet,
    RollingSphereComponent* playerSphere,
    DirectX::XMFLOAT3 localDirection,
    float heightOffset,
    float triggerRadius)
    : GameComponent(game),
    m_Planet(planet),
    m_Sphere(playerSphere),
    m_LocalDirection(localDirection),
    m_HeightOffset(heightOffset),
    m_TriggerRadius(triggerRadius),
    m_IsAttached(false),
    m_WorldPosition(0, 0, 0),
    m_WorldRotation(0, 0, 0)
{
    DirectX::XMStoreFloat4x4(&m_LocalToSphereMat, DirectX::XMMatrixIdentity());
    m_CurrentRotMatrix = DirectX::XMMatrixIdentity();
}

void StickyCollectibleComponent::Initialize()
{
    // Ensure direction is perfectly normalized
    using namespace DirectX;
    XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&m_LocalDirection));
    XMStoreFloat3(&m_LocalDirection, dir);

    // Initial positioning
    Update();
}

void StickyCollectibleComponent::Update()
{
    using namespace DirectX;

    if (!m_IsAttached)
    {
        // -------------------------------------------------------------
        // 1. UPDATE TRANSFORM RELATIVE TO PLANET
        // -------------------------------------------------------------
        XMFLOAT3 pRot = m_Planet->GetRotation();
        XMFLOAT3 pPos = m_Planet->GetPosition();

        XMMATRIX planetRot = XMMatrixRotationY(pRot.y) *
            XMMatrixRotationX(pRot.x) *
            XMMatrixRotationZ(pRot.z);

        // Align component "UP" (0,1,0) to face away from the planet's center
        XMVECTOR up = XMVectorSet(0, 1, 0, 0);
        XMVECTOR localDirVec = XMLoadFloat3(&m_LocalDirection);
        XMVECTOR axis = XMVector3Cross(up, localDirVec);

        float dot = XMVectorGetX(XMVector3Dot(up, localDirVec));
        XMMATRIX alignRot;
        if (dot > 0.9999f) alignRot = XMMatrixIdentity();
        else if (dot < -0.9999f) alignRot = XMMatrixRotationX(XM_PI);
        else {
            axis = XMVector3Normalize(axis);
            float angle = acos(dot);
            alignRot = XMMatrixRotationAxis(axis, angle);
        }

        m_CurrentRotMatrix = alignRot * planetRot;
        m_WorldRotation = ExtractEulerYXZ(m_CurrentRotMatrix);

        // Position = Planet Center + (Rotated Local Direction * Dist)
        float totalDist = m_Planet->GetRadius() + m_HeightOffset;
        XMVECTOR worldDir = XMVector3TransformNormal(localDirVec, planetRot);
        XMVECTOR worldPosVec = XMLoadFloat3(&pPos) + XMVectorScale(worldDir, totalDist);
        XMStoreFloat3(&m_WorldPosition, worldPosVec);

        // -------------------------------------------------------------
        // 2. CHECK FOR COLLISION WITH SPHERE
        // -------------------------------------------------------------
        if (m_Sphere)
        {
            XMFLOAT3 sPos = m_Sphere->GetPosition();
            XMVECTOR spherePosVec = XMLoadFloat3(&sPos);
            float dist = XMVectorGetX(XMVector3Length(worldPosVec - spherePosVec));

            // If center touches the sphere's surface, trigger attachment
            if (dist <= m_TriggerRadius)
            {
                m_IsAttached = true;

                // Capture current world matrix of THIS object
                XMMATRIX myWorld = m_CurrentRotMatrix * XMMatrixTranslationFromVector(worldPosVec);

                // Compute exact World Matrix of the sphere right now
                XMFLOAT3 sRot = m_Sphere->GetRotation();
                XMMATRIX sphereRotMat = XMMatrixRotationY(sRot.y) *
                    XMMatrixRotationX(sRot.x) *
                    XMMatrixRotationZ(sRot.z);
                XMMATRIX sphereWorld = sphereRotMat * XMMatrixTranslationFromVector(spherePosVec);

                // Local relative matrix = MyWorld * Inverse(SphereWorld)
                XMMATRIX invSphere = XMMatrixInverse(nullptr, sphereWorld);
                XMMATRIX localToSphere = myWorld * invSphere;
                XMStoreFloat4x4(&m_LocalToSphereMat, localToSphere);
            }
        }
    }

    if (m_IsAttached && m_Sphere)
    {
        // -------------------------------------------------------------
        // 3. UPDATE TRANSFORM RELATIVE TO SPHERE (POST-ATTACHMENT)
        // -------------------------------------------------------------
        XMFLOAT3 sPos = m_Sphere->GetPosition();
        XMFLOAT3 sRot = m_Sphere->GetRotation();

        XMMATRIX sphereRotMat = XMMatrixRotationY(sRot.y) *
            XMMatrixRotationX(sRot.x) *
            XMMatrixRotationZ(sRot.z);
        XMMATRIX sphereWorld = sphereRotMat * XMMatrixTranslationFromVector(XMLoadFloat3(&sPos));

        // New World Matrix = Local matrix * current Sphere World Matrix
        XMMATRIX localToSphere = XMLoadFloat4x4(&m_LocalToSphereMat);
        XMMATRIX myNewWorld = localToSphere * sphereWorld;

        // Extract translation
        XMVECTOR scale, rotQuat, trans;
        XMMatrixDecompose(&scale, &rotQuat, &trans, myNewWorld);
        XMStoreFloat3(&m_WorldPosition, trans);

        // Extract rotation and convert to Euler Y-X-Z for the MeshRenderer
        m_CurrentRotMatrix = XMMatrixRotationQuaternion(rotQuat);
        m_WorldRotation = ExtractEulerYXZ(m_CurrentRotMatrix);
    }
}

DirectX::XMFLOAT3 StickyCollectibleComponent::GetPosition() const
{
    return m_WorldPosition;
}

DirectX::XMFLOAT3 StickyCollectibleComponent::GetRotation() const
{
    return m_WorldRotation;
}

// Helper to convert arbitrary matrix back to the specific Y-X-Z euler format used by MeshRenderer
DirectX::XMFLOAT3 StickyCollectibleComponent::ExtractEulerYXZ(DirectX::CXMMATRIX m)
{
    using namespace DirectX;
    XMFLOAT4X4 mat;
    XMStoreFloat4x4(&mat, m);

    float sx = mat._23;
    if (sx > 1.0f) sx = 1.0f;
    if (sx < -1.0f) sx = -1.0f;
    float x = asin(sx);
    float cx = cos(x);

    float y, z;
    if (cx > 0.001f)
    {
        y = atan2(-mat._13, mat._33);
        z = atan2(-mat._21, mat._22);
    }
    else // Gimbal lock
    {
        z = 0.0f;
        y = atan2(mat._31, mat._11);
    }

    return XMFLOAT3(x, y, z);
}