#include "PhysicsDebugDraw.h"

PhysicsDebugDraw::PhysicsDebugDraw()
{
}

void PhysicsDebugDraw::DebugDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader, const physx::PxDebugLine& line, Camera& camera)
{
	this->mDevice = device;
	this->mDeviceContext = deviceContext;

	DirectX::XMMATRIX transformationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader->GetBuffer().GetAddressOf());
	cb_vs_vertexshader->data.viewMatrix = DirectX::XMMatrixTranspose(camera.GetViewMatrix());
	cb_vs_vertexshader->data.projectionMatrix = DirectX::XMMatrixTranspose(camera.GetProjectionMatrix());
	cb_vs_vertexshader->data.worldMatrix = worldMatrix;
	//cb_vs_vertexshader->data.wvpMatrix = worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();

	vertices.push_back(Vertex(line.pos0.x, line.pos0.y, line.pos0.z, line.color0, line.color1));
	vertices.push_back(Vertex(line.pos1.x, line.pos1.y, line.pos1.z, line.color0, line.color1));

	if (vertices.size() > 0)
	{
		mesh = new Mesh(mDevice, mDeviceContext, vertices, DirectX::XMMatrixIdentity());
		this->mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		cb_vs_vertexshader->UpdateBuffer();

		mesh->Draw();
		vertices.clear();
		delete mesh;
	}
}
