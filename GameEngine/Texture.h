#pragma once
#include "DX11Includes.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>
#include <assimp/material.h>

enum class TextureStorageType
{
	Invalid,
	None,
	Disk
};

class Texture
{
public:
	Texture();
	Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type);
	Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath, aiTextureType type);
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();
	aiTextureType GetType();
	void CreateTextureDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath);
	void CreateTextureDDSFromWIC(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath);
	void CreateTextureWIC(ID3D11Device* device, const std::string& filePath);
	~Texture();


	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	D3D11_TEXTURE2D_DESC desc;


	std::string texturePath;
	std::string path;

	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};

