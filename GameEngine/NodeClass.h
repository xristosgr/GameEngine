#pragma once

#include "CubeShape.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>
#include "Camera.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"

class NodeClass
{
public:
	NodeClass();
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool visualize = true);
	void DrawNode(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, int& showMode);

	bool isAiActive, isPlayerActive;
	bool isValidPath;
	//bool isVisited;
	bool hasInit;

	//the distance between the current node and the start node
	float gCost;
	//the estimated distance from the current node to the end node
	float hCost;
	//the total cost of the node
	float fCost;

	bool isInCloseList, isInOpenList;
	std::vector<NodeClass*> neighbours;
	NodeClass* parent;

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;

	CubeShape cube;
};

