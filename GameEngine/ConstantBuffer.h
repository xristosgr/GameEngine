#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>
#include "ErrorLogger.h"

template <class T>
class ConstantBuffer
{

public:

	ConstantBuffer() {}

	HRESULT Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext)
	{
		this->deviceContext = deviceContext;
		D3D11_BUFFER_DESC constBufferDesc;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = 0;


		constBufferDesc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));

		constBufferDesc.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&constBufferDesc, nullptr, constantBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create constant buffer.");
			return hr;
		}
	}

	bool UpdateBuffer()
	{

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->deviceContext->Map(this->constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map constant buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->deviceContext->Unmap(this->constantBuffer.Get(), 0);


		return true;
	}
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetBuffer()
	{
		return constantBuffer;
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
public:
	T data;

};