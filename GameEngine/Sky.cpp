#include "Sky.h"

Sky::Sky()
{
	pos = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(100.0, 100.0, 100.0);
	rot = DirectX::XMFLOAT3(0.0, 0.0, 0.0);

	//color = DirectX::XMFLOAT3(0.64f, 0.805f, 0.74f);
	color = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Sky::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	model.loadAsync = true;
	model.Initialize(".//Data/Objects/skyDome.obj", device, deviceContext, cb_vs_vertexshader, false);
}

void Sky::DrawGui(std::string name)
{
	ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	ImGui::DragFloat3("Scale", &scale.x, 0.005f);

	ImGui::DragFloat3("color", &color.x, 0.005f);
}

void Sky::Draw(Camera& camera)
{
	DirectX::XMMATRIX matrix_scale;
	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;
	DirectX::XMMATRIX worldMatrix;

	pos = camera.pos;
	matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	matrix_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	worldMatrix = matrix_scale * matrix_rotate * matrix_translate;

	model.Draw(worldMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}
