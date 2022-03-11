#include "Texture.h"
#include "ErrorLogger.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <ScreenGrab.h>
Texture::Texture()
{
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	this->type = type;
	
	CreateTextureWIC(device, filePath);
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return this->textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return this->textureView.GetAddressOf();
}

aiTextureType Texture::GetType()
{
	return this->type;
}

void Texture::CreateTextureDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath)
{
	HRESULT hr;

	hr = DirectX::CreateDDSTextureFromFileEx(device,
		deviceContext,
		StringHelper::StringToWide(filePath).c_str(),
		0Ui64,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_GENERATE_MIPS,
		false,
		this->texture.GetAddressOf(),
		textureView.GetAddressOf()
	);
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture from file: " + filePath);
	path = filePath;
}

void Texture::CreateTextureWIC(ID3D11Device* device, const std::string& filePath)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
	if (FAILED(hr))
	{
		COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
	}
	path = filePath;

}

Texture::~Texture()
{
}
