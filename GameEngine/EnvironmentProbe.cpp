#include "EnvironmentProbe.h"

EnvironmentProbe::EnvironmentProbe()
{
	//direction[0] = DirectX::XMFLOAT3(-1000.0f, 0.02f, 1.0f);
	//direction[1] = DirectX::XMFLOAT3(1000.0f, -0.02f, 1.0f);
	//direction[2] = DirectX::XMFLOAT3(0.0f, 1000.0f, 1.0f);
	//direction[3] = DirectX::XMFLOAT3(0.0f, -1000.0f, 0.0f);
	//direction[4] = DirectX::XMFLOAT3(1.0f, 0.02f, -1000.0f);
	//direction[5] = DirectX::XMFLOAT3(1.0f, -0.02f, 1000.0f);


	direction[0] = DirectX::XMFLOAT3(-1000.0f, 0.02f, 1.0f);
	direction[1] = DirectX::XMFLOAT3(1000.0f, -0.02f, 1.0f);
	direction[2] = DirectX::XMFLOAT3(0.0f, 1000.0f, 0.0f);
	direction[3] = DirectX::XMFLOAT3(0.0f, -1000.0f, 0.0f);
	direction[4] = DirectX::XMFLOAT3(1.0f, 0.02f, -1000.0f);
	direction[5] = DirectX::XMFLOAT3(1.0f, -0.02f, 1000.0f);

	for (int i = 0; i < 6; ++i)
		fovs[i] = 90.0f;

	pos = DirectX::XMFLOAT3(0.0f, 2.0f, 0.0f);
	//pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(0.4, 0.4, 0.4);
	rot = DirectX::XMFLOAT3(1.5, 0.0, 0.0);
	recalculate = true;
	index = 0;
}

bool EnvironmentProbe::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, int width, int height)
{
	recalculate = true;
	if(!model.Initialize(".//Data/Objects/skyDome.obj", device, deviceContex, cb_vs_vertexshader, false))
		return false;

	model.LoadModel(".//Data/Objects/skyDome.obj");
	
	return true;
}

Camera* EnvironmentProbe::GetCamera()
{
	return nullptr;
}

void EnvironmentProbe::UpdateCamera()
{
		for (int i = 0; i < 6; i++)
		{
			camera[i].SetLookAtPos(DirectX::XMFLOAT3(direction[i].x + pos.x, direction[i].y + pos.y, direction[i].z + pos.z));

			camera[i].SetPosition(pos.x, pos.y, pos.z);

			camera[i].PerspectiveFov(fovs[i], 1, 0.1f, 10000.0f);

			viewMatrices[i] = camera[i].GetViewMatrix();
			projectionMatrices[i] = camera[i].GetProjectionMatrix();

		}
}

void EnvironmentProbe::Draw(Camera& camera)
{
	DirectX::XMMATRIX m_scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX m_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	DirectX::XMMATRIX m_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	const DirectX::XMMATRIX worldMatrix = m_scale * m_rotate * m_translate;

	model.Draw(worldMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}

void EnvironmentProbe::DrawGui(std::string name)
{
	ImGui::DragFloat3("pos", &pos.x, 0.005f);

	ImGui::DragFloat3("View0", &direction[0].x);
	ImGui::DragFloat3("View1", &direction[1].x);
	ImGui::DragFloat3("View2", &direction[2].x);
	ImGui::DragFloat3("View3", &direction[3].x);
	ImGui::DragFloat3("View4", &direction[4].x);
	ImGui::DragFloat3("View5", &direction[5].x);

	ImGui::DragFloat("fov0", &fovs[0]);
	ImGui::DragFloat("fov1", &fovs[1]);
	ImGui::DragFloat("fov2", &fovs[2]);
	ImGui::DragFloat("fov3", &fovs[3]);
	ImGui::DragFloat("fov4", &fovs[4]);
	ImGui::DragFloat("fov5", &fovs[5]);

	ImGui::DragInt("Index", &index);
	if(ImGui::Button("UpdateProbe"))
	{
		recalculate = true;
	}
}
