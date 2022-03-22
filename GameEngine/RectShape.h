#pragma once

#include "Shapes.h"

class RectShape : public Shapes
{
public:
	RectShape();
	~RectShape();

	void Initialize(ID3D11Device* device, UINT32 windowWidth, UINT32 windowHeight);

	virtual void CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath) override;
	virtual void SetRenderTexture(ID3D11DeviceContext* deviceContext, RenderTexture& renderTexture) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) override;
	virtual void DrawGUI(std::string name) override;
};

