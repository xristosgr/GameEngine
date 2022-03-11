#include "Frustum.h"

Frustum::Frustum()
{
}

void Frustum::ConstructFrustum(float screenDepth, DirectX::XMMATRIX& view, DirectX::XMMATRIX& Proj)
{
    float r;
    float zMinimum;
    // Left Frustum Plane
    // Add first column of the matrix to the fourth column
    DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(view, Proj);

    DirectX::XMStoreFloat4x4(&viewProj, viewProjMatrix);
    DirectX::XMFLOAT4X4 viewFloat4x4;
    DirectX::XMFLOAT4X4 projFloat4x4;
    DirectX::XMStoreFloat4x4(&viewFloat4x4, view);
    DirectX::XMStoreFloat4x4(&projFloat4x4, Proj);

    zMinimum = -projFloat4x4._43 / projFloat4x4._33;
    r = screenDepth / (screenDepth - zMinimum);
    projFloat4x4._33 = r;
    projFloat4x4._43 = -r * zMinimum;

    Proj = DirectX::XMLoadFloat4x4(&projFloat4x4);
    view = DirectX::XMLoadFloat4x4(&viewFloat4x4);

    viewProjMatrix = DirectX::XMMatrixMultiply(view, Proj);
    DirectX::XMStoreFloat4x4(&viewProj, viewProjMatrix);
    // Calculate near plane of frustum.
    FrustumPlane[0].x = viewProj._14 + viewProj._13;
    FrustumPlane[0].y = viewProj._24 + viewProj._23;
    FrustumPlane[0].z = viewProj._34 + viewProj._33;
    FrustumPlane[0].w = viewProj._44 + viewProj._43;

    m_planes[0] = DirectX::XMLoadFloat4(&FrustumPlane[0]);
    m_planes[0] = DirectX::XMPlaneNormalize(m_planes[0]);


    // Calculate far plane of frustum.
    FrustumPlane[1].x = viewProj._14 - viewProj._13;
    FrustumPlane[1].y = viewProj._24 - viewProj._23;
    FrustumPlane[1].z = viewProj._34 - viewProj._33;
    FrustumPlane[1].w = viewProj._44 - viewProj._43;
    m_planes[1] = DirectX::XMLoadFloat4(&FrustumPlane[1]);
    m_planes[1] = DirectX::XMPlaneNormalize(m_planes[1]);


    // Calculate left plane of frustum.
    FrustumPlane[2].x = viewProj._14 + viewProj._11;
    FrustumPlane[2].y = viewProj._24 + viewProj._21;
    FrustumPlane[2].z = viewProj._34 + viewProj._31;
    FrustumPlane[2].w = viewProj._44 + viewProj._41;
    m_planes[2] = DirectX::XMLoadFloat4(&FrustumPlane[2]);
    m_planes[2] = DirectX::XMPlaneNormalize(m_planes[2]);


    // Calculate right plane of frustum.
    FrustumPlane[3].x = viewProj._14 - viewProj._11;
    FrustumPlane[3].y = viewProj._24 - viewProj._21;
    FrustumPlane[3].z = viewProj._34 - viewProj._31;
    FrustumPlane[3].w = viewProj._44 - viewProj._41;
    m_planes[3] = DirectX::XMLoadFloat4(&FrustumPlane[3]);
    m_planes[3] = DirectX::XMPlaneNormalize(m_planes[3]);


    // Calculate top plane of frustum.
    FrustumPlane[4].x = viewProj._14 - viewProj._12;
    FrustumPlane[4].y = viewProj._24 - viewProj._22;
    FrustumPlane[4].z = viewProj._34 - viewProj._32;
    FrustumPlane[4].w = viewProj._44 - viewProj._42;
    m_planes[4] = DirectX::XMLoadFloat4(&FrustumPlane[4]);
    m_planes[4] = DirectX::XMPlaneNormalize(m_planes[4]);


    // Calculate bottom plane of frustum.
    FrustumPlane[5].x = viewProj._14 + viewProj._12;
    FrustumPlane[5].y = viewProj._24 + viewProj._22;
    FrustumPlane[5].z = viewProj._34 + viewProj._32;
    FrustumPlane[5].w = viewProj._44 + viewProj._42;
    m_planes[5] = DirectX::XMLoadFloat4(&FrustumPlane[5]);
    m_planes[5] = DirectX::XMPlaneNormalize(m_planes[5]);
}

bool Frustum::CheckRect(float& xCenter, float& yCenter, float& zCenter, float xSize, float ySize, float zSize)
{
    for (int i = 0; i < 6; i++)
    {
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter - xSize), (yCenter - ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter + xSize), (yCenter - ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter - xSize), (yCenter + ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter - xSize), (yCenter - ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter + xSize), (yCenter + ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter + xSize), (yCenter - ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter - xSize), (yCenter + ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        if (DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(m_planes[i], DirectX::XMVECTOR{ (xCenter + xSize), (yCenter + ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        checkFrustum = false;
        return checkFrustum;
    }
    checkFrustum = true;
    return checkFrustum;
}
