#include "Light.h"

Light::Light()
{
	camera = std::make_unique<Camera>();

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
	volumeScale = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
	coneRot = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
}

void Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = cb_vs_vertexshader;

	sphere.loadAsync = true;
	sphere.isTextured = false;
	sphere.Initialize(".//Data/Objects/skyDome.obj", device, deviceContext, cb_vs_vertexshader, false);
	//sphere.Initialize(".//Data/Objects/lightVolume.obj", device, deviceContext, cb_vs_vertexshader, false);
}

void Light::SetupCamera(int windowWidth, int windowHeight)
{
	m_screenWidth = windowWidth;
	m_screenHeight = windowHeight;
	camera->SetPosition(pos.x, pos.y, pos.z);
	camera->SetLookAtPos(direction);

	if (lightType == 2.0f)
		camera->OrthographicFov(windowWidth, windowHeight, nearZ, farZ);
	else
		camera->PerspectiveFov(90.0f, 1, 0.1f, 60.0f);

	lightViewMatrix = camera->GetViewMatrix();
	lightProjectionMatrix = camera->GetProjectionMatrix();
}

void Light::UpdateCamera()
{
	camera->SetPosition(pos);
	//camera[i].pos = pos;
	if (lightType == 2.0f)
	{
		camera->OrthographicFov(m_screenWidth / 30, m_screenHeight / 30, nearZ, farZ);
		camera->SetLookAtPos(DirectX::XMFLOAT3(direction.x + pos.x, direction.y + pos.y, direction.z + pos.z));
	}
	else 
	{
		camera->PerspectiveFov(fov, dimensions, nearZ, farZ);
		camera->SetLookAtPos(direction);
	}


	//camera[i].SetPosition(pos.x, pos.y, pos.z);
	lightViewMatrix = camera->GetViewMatrix();
	lightProjectionMatrix = camera->GetProjectionMatrix();
}

Camera* Light::GetCamera()
{
	return camera.get();
}

void Light::DrawGui(std::string name)
{
	ImGui::Checkbox("Enable", &isLightEnabled);
	ImGui::Checkbox("CastShadow", &bShadow);

	ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	ImGui::DragFloat3("Scale", &scale.x, 0.005f);

	ImGui::DragFloat3("lightColor", &lightColor.x, 0.005f);
	ImGui::DragFloat3("emissionColor", &emissionColor.x, 0.005f);
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
	//DrawVolume(camera);
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

	volumeScale = DirectX::XMFLOAT3(radius, radius, radius);

	matrix_scale = DirectX::XMMatrixScaling(this->volumeScale.x, this->volumeScale.y, this->volumeScale.z);
	matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	matrix_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
	sphere.Draw(worldMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}
