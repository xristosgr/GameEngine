#pragma once
#pragma once
#include <d3d11.h>
#include<DirectXMath.h>
#include<string>
#include<vector>

class Frustum
{
public:
	Frustum();

	void ConstructFrustum(float screenDepth, DirectX::XMMATRIX& view, DirectX::XMMATRIX& Proj);
	bool CheckRect(float& xCenter, float& yCenter, float& zCenter, float xSize, float ySize, float zSize);
	DirectX::XMFLOAT4 FrustumPlane[6];

	bool checkFrustum = false;

private:

	DirectX::XMFLOAT4X4 viewProj;
	std::vector<DirectX::XMFLOAT3> modelAABB;

	DirectX::XMVECTOR m_planes[6];
};

