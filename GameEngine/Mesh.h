#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"
#include"VertexBoneData.h"

class Mesh
{
public:
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, DirectX::XMMATRIX transformMatrix);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, DirectX::XMMATRIX transformMatrix);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, std::vector<VertexBoneData>& bones, DirectX::XMMATRIX transformMatrix);

	void LoadTextures(std::vector<Texture>& textures);



	void Draw(Texture* text = nullptr);
	const DirectX::XMMATRIX& GetTranformMatrix();

	VertexBuffer<Vertex> vertexBuffer;
	VertexBuffer<VertexBoneData> boneDataBuffer;
	IndexBuffer indexBuffer;
	std::vector<Texture> textures;
private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	DirectX::XMMATRIX transformMatrix;

	Texture tex;
};

