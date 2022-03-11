#pragma once
#include "NodeClass.h"
#include <vector>
#include<future>
#include <PxPhysicsAPI.h>
#include <thread>
#include "CubeShape.h"

class GridClass
{
public:
	GridClass();
	void InitializeBoundsVolume(ID3D11Device* device);
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void DrawGrid(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, std::vector<NodeClass>& validNodes);
	void DrawBounds(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void FindNeighbours(NodeClass& currentNode, std::vector<NodeClass>& validNodes);
	void CreatePathGrid(std::vector<NodeClass>& validNodes);
	void SetupGridBounds();
	void DrawGUI();

	XMFLOAT3 bounds;
	XMFLOAT3 pos;

	std::vector<NodeClass> nodes;
	bool showGrid;
	bool isReady;
	bool bCreateMesh;
	bool hasFinished;
	int showMode;

	CubeShape cubeBoundsVolume;


private:
	void InitThread();

	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _deviceContext;
	DirectX::XMMATRIX _transformMatrix;
	ConstantBuffer<CB_VS_vertexshader> _cb_vs_vertexshader;

	std::future<void> m_async;
};

