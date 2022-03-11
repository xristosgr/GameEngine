#include "NodeClass.h"

NodeClass::NodeClass()
{
	hasInit = false;
	//isActive = false;
	isValidPath = true;
	//isVisited = false;
	parent = nullptr;
	parent = nullptr;

	isInCloseList = false;
	isInOpenList = false;

	gCost = 0.0f;
	hCost = 0.0f;
	fCost = 0.0f;

	scale = DirectX::XMFLOAT3(0.5,0.3, 0.5);
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void NodeClass::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool visualize)
{
	isAiActive = false;
	isPlayerActive = false;
	isValidPath = true;
	//isVisited = false;
	parent = nullptr;

	scale = DirectX::XMFLOAT3(0.5, 0.3, 0.5);
	cube.Initialize(device.Get());
	cube.scale = scale;
	cube.pos = pos;

	hasInit = true;
}

void NodeClass::DrawNode(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, int& showMode)
{
	if (hasInit)
	{
		if (showMode == 0)
		{
			if (isInCloseList)
				cube.Draw(deviceContext, camera, cb_vs_vertexshader);
		}
		else
		{
			if (isValidPath)
				cube.Draw(deviceContext, camera, cb_vs_vertexshader);
		}

		//if (isAiActive)
		//{
		//	for(auto neighour : neighbours)
		//		neighour->cube.Draw(deviceContext, camera, cb_vs_vertexshader);
		//}
			
	}
}
