#include "Sky.h"

Sky::Sky()
{
	pos = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(100.0, 100.0, 100.0);
	rot = DirectX::XMFLOAT3(0.0, 0.0, 0.0);

	//color = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	color = DirectX::XMFLOAT3(1.67f, 1.29f, 3.0f);

	apexColor = DirectX::XMFLOAT4(0.71f, 0.95f, 0.845f,1.0f);
	centerColor = DirectX::XMFLOAT4(0.72f, 0.955f, 0.84f,1.0f);

	//apexColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//centerColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Sky::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	//cube.Initialize(device);
	model.loadAsync = true;
	model.bConvertCordinates = true;
	model.Initialize(".//Data/Objects/skyDome1.obj", device, deviceContext, cb_vs_vertexshader, false);
}

void Sky::DrawGui(std::string name)
{
	ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	ImGui::DragFloat3("Scale", &scale.x, 0.005f);

	ImGui::DragFloat4("color", &color.x, 0.005f);

	ImGui::DragFloat4("apexColor", &apexColor.x, 0.005f);
	ImGui::DragFloat4("centerColor", &centerColor.x, 0.005f);
}

void Sky::Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	DirectX::XMMATRIX matrix_scale;
	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;
	DirectX::XMMATRIX worldMatrix;

	matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	matrix_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
	model.Draw(worldMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}
