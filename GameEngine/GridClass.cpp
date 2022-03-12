#include "GridClass.h"

GridClass::GridClass()
{
	showGrid = false;
	isReady = false;
	bCreateMesh = false;
	hasFinished = false;
	//bounds = DirectX::XMFLOAT3(11.3f, 1.0f, 5.0f);
	bounds = DirectX::XMFLOAT3(34.2f, 1.0f, 39.3f);
	showMode = 0;
}

void GridClass::InitializeBoundsVolume(ID3D11Device* device)
{
	cubeBoundsVolume.Initialize(device);
}

void GridClass::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	_device = device;
	_deviceContext = deviceContext;
	_transformMatrix = transformMatrix;
	_cb_vs_vertexshader = cb_vs_vertexshader;

	//InitThread();
	m_async = std::async(std::launch::async, &GridClass::InitThread, this);
	//InitThread();
}

void GridClass::DrawGrid(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, std::vector<NodeClass>& validNodes)
{
	//if (showGrid)
	//{
		for (int i = 0; i < validNodes.size(); ++i)
		{
			validNodes[i].DrawNode(deviceContext, camera, cb_vs_vertexshader,showMode);
		}


	//}
}

void GridClass::DrawBounds(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	cubeBoundsVolume.Draw(deviceContext, camera, cb_vs_vertexshader);
}

void GridClass::FindNeighbours(NodeClass& currentNode, std::vector<NodeClass>& validNodes)
{
	for (int i = 0; i < validNodes.size(); ++i)
	{

		if (currentNode.pos.x >= validNodes[i].pos.x - 1 && currentNode.pos.x <= validNodes[i].pos.x + 1 &&
			currentNode.pos.y >= validNodes[i].pos.y - 1 && currentNode.pos.y <= validNodes[i].pos.y + 1 &&
			currentNode.pos.z >= validNodes[i].pos.z - 1 && currentNode.pos.z <= validNodes[i].pos.z + 1)
		{
			currentNode.neighbours.push_back(&validNodes[i]);
		}


		//OutputDebugStringA(("Size = " + std::to_string(nodes[i].neighbours.size()) + "\n").c_str());
	}
}

void GridClass::CreatePathGrid(std::vector<NodeClass>& validNodes)
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].isValidPath)
		{
			validNodes.push_back(nodes[i]);
		}
	}
	//OutputDebugStringA(("SIZE = " + std::to_string(validNodes.size()) + "\n").c_str());

	if (!validNodes.empty())
	{
		for (int i = 0; i < validNodes.size(); ++i)
		{

			if (validNodes[i].isValidPath)
				FindNeighbours(validNodes[i], validNodes);
		}
	}
	
	hasFinished = true;
	OutputDebugStringA("GRID CREATED!!!\n");
}

void GridClass::SetupGridBounds()
{
	cubeBoundsVolume.scale = bounds;
}

void GridClass::DrawGUI()
{
	ImGui::DragFloat3("Bounds", &bounds.x, 0.1f);
	ImGui::InputInt("mode", &showMode);
}

void GridClass::InitThread()
{
	for (int y = -bounds.y; y < bounds.y; ++y)
	{
		for (int z = -bounds.z; z < bounds.z; ++z)
		{
			for (int x = -bounds.x; x < bounds.x; ++x)
			{
				nodes.push_back(NodeClass());
				nodes[nodes.size() - 1].pos = DirectX::XMFLOAT3(pos.x - x, pos.y - y, pos.z - z);
				nodes[nodes.size() - 1].Initialize(_device, _deviceContext, _transformMatrix, _cb_vs_vertexshader, true);
				

				//nodes[nodes.size() - 1].shape.pos = nodes[nodes.size() - 1].pos;
			}
		}
	}
	isReady = true;
}
