#include "Shapes.h"

void Shapes::CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath)
{
	texture.CreateTextureDDS(device, deviceContext, filePath);
}

void Shapes::SetRenderTexture(ID3D11DeviceContext* deviceContext, RenderTexture& renderTexture)
{
	deviceContext->PSSetShaderResources(0, 1, &renderTexture.shaderResourceView);
}

void Shapes::Draw(ID3D11DeviceContext* deviceContext,Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	//pos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMMATRIX m_scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX m_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	DirectX::XMMATRIX m_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	if (indexBuffer.IndexCount() > 0)
		deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	if (texture.GetTextureResourceView())
		deviceContext->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());

	deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetBuffer().GetAddressOf());
	cb_vs_vertexshader.data.projectionMatrix = DirectX::XMMatrixTranspose(camera.GetProjectionMatrix());
	cb_vs_vertexshader.data.viewMatrix = DirectX::XMMatrixTranspose(camera.GetViewMatrix());
	cb_vs_vertexshader.data.worldMatrix = m_scale * m_rotate * m_translate;
	cb_vs_vertexshader.data.worldMatrix = DirectX::XMMatrixTranspose(cb_vs_vertexshader.data.worldMatrix);

	cb_vs_vertexshader.UpdateBuffer();
	
	if (indexBuffer.IndexCount() > 0)
		deviceContext->DrawIndexed(indexBuffer.IndexCount(), 0, 0);
	else
		deviceContext->Draw(vertexBuffer.VertexCount(), 0);
	
	
}

void Shapes::Draw(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& viewMatrix, DirectX::XMMATRIX& projectionMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	DirectX::XMMATRIX m_scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX m_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	DirectX::XMMATRIX m_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	if(indexBuffer.IndexCount()>0)
		deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	//if (texture.GetTextureResourceView())
	//	deviceContext->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());

	deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetBuffer().GetAddressOf());
	cb_vs_vertexshader.data.projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);
	cb_vs_vertexshader.data.viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	cb_vs_vertexshader.data.worldMatrix = m_scale * m_rotate * m_translate;
	cb_vs_vertexshader.data.worldMatrix = DirectX::XMMatrixTranspose(cb_vs_vertexshader.data.worldMatrix);

	cb_vs_vertexshader.UpdateBuffer();

	if (indexBuffer.IndexCount() > 0)
		deviceContext->DrawIndexed(indexBuffer.IndexCount(), 0, 0);
	else
		deviceContext->Draw(vertexBuffer.VertexCount(), 0);
	
}

void Shapes::DrawGUI(std::string name)
{
	//ImGui::Begin(name.c_str());

	ImGui::DragFloat3("rot", &rot.x, 0.01f);
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);

	//ImGui::End();
}
