#pragma once
#include "Shapes.h"

class CubeShape : public Shapes
{
public:
	CubeShape();
	~CubeShape();

	void Initialize(ID3D11Device* device);
	virtual void CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) override;
	virtual void DrawGUI(std::string name) override;
};

