#include "Light.h"

Light::Light()
{
	pos = DirectX::XMFLOAT3(4.925, 2.4, 1.6);
	scale = DirectX::XMFLOAT3(0.05, 0.05, 0.05);
	rot = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	posOffset = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	lightColor = DirectX::XMFLOAT3(50.0, 50.0, 50.0);
	fov = 90.0f;
	lightStrenth = 10.0f;
	radius = 13.0f;
	lightAttenuation = 1.0f;
	dimensions = 1;
	cutOff = 0.37f;
	lightType = 0.0f;
	offset = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	direction = DirectX::XMFLOAT3(-1.21, 0.0, -12.69);


	conePos = DirectX::XMFLOAT3(0, 0, 0);
	coneScale = DirectX::XMFLOAT3(1, 1, 1);
	coneRot = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
}

void Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = cb_vs_vertexshader;

	sphere.loadAsync = true;
	volume.loadAsync = true;
	sphere.Initialize(".//Data/Objects/skyDome.obj", device, deviceContext, cb_vs_vertexshader, false);
	//volume.Initialize(".//Data/Objects/volume.obj", device, deviceContext, cb_vs_vertexshader, false);
	//cube.Initialize(device);

	//m_shadowMap.Initialize(device, 1024, 1024);
}

void Light::SetupCamera(int windowWidth, int windowHeight)
{
	camera.SetPosition(pos.x, pos.y, pos.z);
	camera.SetLookAtPos(direction);
	camera.PerspectiveFov(90.0f, 1, 0.1f, 60.0f);

	lightViewMatrix = camera.GetViewMatrix();
	lightProjectionMatrix = camera.GetProjectionMatrix();
}

void Light::UpdateCamera()
{
	camera.SetPosition(pos);
	//camera[i].pos = pos;
	camera.PerspectiveFov(fov, dimensions, nearZ, farZ);
	//camera.SetLookAtPos(XMFLOAT3(direction.x + pos.x, direction.y + pos.y, direction.z + pos.z));
	camera.SetLookAtPos(direction);
	//camera[i].SetPosition(pos.x, pos.y, pos.z);
	lightViewMatrix = camera.GetViewMatrix();
	lightProjectionMatrix = camera.GetProjectionMatrix();
}

void Light::DrawGui(std::string name)
{
	//ImGui::Begin(name.c_str());

	ImGui::Checkbox("Enable", &isLightEnabled);
	ImGui::Checkbox("CastShadow", &bShadow);

	ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	ImGui::DragFloat3("Scale", &scale.x, 0.005f);

	ImGui::DragFloat3("lightColor", &lightColor.x, 0.005f);
	//ImGui::DragFloat("light strength", &lightStrenth, 0.005f, 0.0f, 100.0f);
	//ImGui::DragFloat("light attenuation", &lightAttenuation, 0.005f, 0.0f, 100.0f);

	ImGui::DragFloat3("direction", &direction.x, 0.01f);
	ImGui::DragFloat3("SpotDir", &SpotDir.x, 0.05f);
	//ImGui::DragFloat3("offset", &offset.x, 0.05f);
	ImGui::DragFloat("radius", &radius, 0.05f);
	ImGui::DragFloat("cutOff", &cutOff, 0.01f);
	ImGui::InputFloat("lightType", &lightType);

	ImGui::NewLine();
	ImGui::DragFloat("nearZ", &nearZ, 0.05f);
	ImGui::DragFloat("farZ", &farZ, 0.05f);
	ImGui::DragFloat("fov", &fov, 0.05f);
	ImGui::DragFloat("dimmensions", &dimensions, 0.05f);




	//ImGui::DragFloat3("coneRotate", &coneRot.x, 0.005f);
	//ImGui::DragFloat3("coneTranslation", &conePos.x, 0.005f);
	//ImGui::DragFloat3("coneScale", &coneScale.x, 0.005f);

	//ImGui::End();
}

void Light::Draw(Camera& camera)
{
	DirectX::XMMATRIX matrix_scale;
	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;
	DirectX::XMMATRIX worldMatrix;

	matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	matrix_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	worldMatrix = matrix_scale * matrix_rotate * matrix_translate;

	sphere.Draw(worldMatrix,camera.GetViewMatrix(),camera.GetProjectionMatrix());

	//sphere.pos = pos;
	//sphere.rot = rot;
	//sphere.scale = scale;
	//cube.Draw(deviceContext, camera, cb_vs_vertexshader);
}

void Light::DrawVolume(Camera& camera)
{
	DirectX::XMMATRIX matrix_scale;
	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;
	DirectX::XMMATRIX worldMatrix;

	matrix_scale = DirectX::XMMatrixScaling(this->coneScale.x, this->coneScale.y, this->coneScale.z);
	matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(coneRot.x, coneRot.y, coneRot.z);
	matrix_translate = DirectX::XMMatrixTranslation(conePos.x, conePos.y, conePos.z);
	worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
	volume.Draw(worldMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}