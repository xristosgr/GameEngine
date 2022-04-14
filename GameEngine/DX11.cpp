#include "DX11.h"
#include <dxgi1_6.h>
#include <math.h>
#include <random>

DX11::DX11()
{
}

bool DX11::Initialize(HWND hwnd, Camera& camera, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;


	return true;
}



bool DX11::InitializeDirectX(HWND hwnd)
{
	try
	{
		HRESULT hr;
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		UINT MSSA_COUNT = 1;
		UINT MSSA_Quality = 0;

		//Device and Swapchain Creation
		DXGI_SWAP_CHAIN_DESC1 scd1 = { 0 };
		scd1.Width = lround(this->windowWidth);		// Match the size of the window.
		scd1.Height = lround(this->windowWidth);
		scd1.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		//scd1.Format = DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM;
		scd1.Stereo = false;
		scd1.SampleDesc.Count = MSSA_COUNT;
		scd1.SampleDesc.Quality = MSSA_Quality;
		scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd1.BufferCount = 3;									//NOTE: Use double-buffering to minimize latency.
		scd1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Microsoft Store apps must use this SwapEffect.
		scd1.Flags = 0;
		scd1.Scaling = DXGI_SCALING_NONE;
		scd1.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC scdFullScreen = { 0 };
		scdFullScreen.RefreshRate.Numerator = 60;
		scdFullScreen.RefreshRate.Denominator = 1;
		scdFullScreen.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		scdFullScreen.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scdFullScreen.Windowed = TRUE;

		D3D_FEATURE_LEVEL featureLevels[]
		{
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
		};
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D3D_FEATURE_LEVEL  FeatureLevelsSupported;
		D3D_FEATURE_LEVEL m_d3dFeatureLevel(D3D_FEATURE_LEVEL_11_1);
		hr = D3D11CreateDevice(
			nullptr,					// Specify nullptr to use the default adapter.
			D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
			0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
			creationFlags,				// Set debug and Direct2D compatibility flags.
			featureLevels,				// List of feature levels this app can support.
			ARRAYSIZE(featureLevels),	// Size of the list above.
			D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
			&device,					// Returns the Direct3D device created.
			&m_d3dFeatureLevel,			// Returns feature level of device created.
			&deviceContext					// Returns the device immediate context.
		);

		COM_ERROR_IF_FAILED(hr, "Failed to create device.");

		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		device.As(&dxgiDevice);

		Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
		dxgiDevice->GetAdapter(&dxgiAdapter);

		Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain1;
		hr = dxgiFactory->CreateSwapChainForHwnd(device.Get(), hwnd, &scd1, &scdFullScreen, nullptr, &swapchain1);

		COM_ERROR_IF_FAILED(hr, "Failed to create swapchain.");

		swapchain1.As(&swapchain);

		if (swapchain != nullptr)
		{
			hr = swapchain->ResizeBuffers(scd1.BufferCount, this->windowWidth, this->windowHeight, scd1.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
			COM_ERROR_IF_FAILED(hr, "Failed to create swapchain.");
		}

		//if (swapchain != nullptr)
		//{
		//	hr = swapchain->ResizeBuffers(scd1.BufferCount, this->windowWidth, this->windowHeight, scd1.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		//	COM_ERROR_IF_FAILED(hr, "Failed to create swapchain.");
		//}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//Create Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight, 1U, 1U, D3D11_BIND_DEPTH_STENCIL);
		depthStencilDesc.SampleDesc.Count = MSSA_COUNT;
		depthStencilDesc.SampleDesc.Quality = MSSA_Quality;
		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");


		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (MSSA_COUNT > 1)
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), &depthStencilViewDesc, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		//depthstencildesc.StencilEnable = true;
		//depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//DepthStencil state for 2D rendering
		depthstencildesc.DepthEnable = false;
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState2D.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state 2D.");

		depthstencildesc.StencilEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilDeferredPassState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
		//Create the Viewport and Set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));

		this->viewport = viewport;
		this->deviceContext->RSSetViewports(1, &this->viewport);

		//Create Rasterizer State

		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		if (MSSA_COUNT > 1)
		{
			rasterizerDesc.MultisampleEnable = true;
			rasterizerDesc.AntialiasedLineEnable = true;
		}
		else
		{
			rasterizerDesc.MultisampleEnable = false;
			rasterizerDesc.AntialiasedLineEnable = false;
		}

		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerStateFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create blend state
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;


		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;


		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");


		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0] = rtbd;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;

		hr = this->device->CreateBlendState(&blendDesc, this->deferredLightBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0] = rtbd;
		hr = this->device->CreateBlendState(&blendDesc, this->AdditiveBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		rtbd.BlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;
		hr = this->device->CreateBlendState(&blendDesc, this->NoBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.BorderColor[0] = 0;
		sampDesc.BorderColor[1] = 0;
		sampDesc.BorderColor[2] = 0;
		sampDesc.BorderColor[3] = 0;
		sampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.MipLODBias = 0.0f;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_Wrap.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		//sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		//sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_Clamp.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_point.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_MipMap.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool DX11::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		//{"TEXCOORD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1  }
	};
	UINT numElements = ARRAYSIZE(layout);
	initVSShader(&vs2D, device, L"Vs2D.cso", layout, &numElements);
	initVSShader(&testVS, device, L"TestVertex.cso", layout, &numElements);


	D3D11_INPUT_ELEMENT_DESC deferredLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(deferredLayout);

	initVSShader(&volumetricLightVS, device, L"VolumetricLightVS.cso", deferredLayout, &numElements);
	initVSShader(&deferredVS, device, L"DeferredVS.cso", deferredLayout, &numElements);

	D3D11_INPUT_ELEMENT_DESC pbrLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(pbrLayout);
	initVSShader(&pbrVS, device, L"PbrVS.cso", pbrLayout, &numElements);

	D3D11_INPUT_ELEMENT_DESC animlayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"WEIGHTS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BONE_IDs", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT,1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(animlayout);
	initVSShader(&animVS, device, L"AnimVS.cso", animlayout, &numElements);
	initVSShader(&depthAnimVS, device, L"DepthAnimVS.cso", animlayout, &numElements);
	initVSShader(&animDeferredVS, device, L"AnimDeferredVS.cso", animlayout, &numElements);

	D3D11_INPUT_ELEMENT_DESC depthLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(depthLayout);
	initVSShader(&depthVS, device, L"DepthVS.cso", depthLayout, &numElements);
	initVSShader(&horizontalBlurVS, device, L"HorizontalBlurVS.cso", depthLayout, &numElements);
	initVSShader(&verticalBlurVS, device, L"VerticalBlurVS.cso", depthLayout, &numElements);
	initVSShader(&shadowHorizontalBlurVS, device, L"ShadowHorizontalVS.cso", depthLayout, &numElements);
	initVSShader(&shadowVerticalBlurVS, device, L"ShadowVerticalVS.cso", depthLayout, &numElements);

	D3D11_INPUT_ELEMENT_DESC shadowLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	numElements = ARRAYSIZE(shadowLayout);

	initVSShader(&shadowVS, device, L"ShadowVS.cso", shadowLayout, &numElements);

	initPSShader(&depthPS, device, L"DepthPS.cso");
	initPSShader(&testPS, device, L"TestPixel.cso");
	initPSShader(&colorPS, device, L"ColorPS.cso");
	initPSShader(&lightPS, device, L"LightPS.cso");
	initPSShader(&pbrPS, device, L"PbrPS.cso");
	initPSShader(&deferredLightPassPS, device, L"DeferredLightPassPS.cso");
	initPSShader(&deferredSpotLightPS, device, L"DeferredSpotLightPS.cso");
	initPSShader(&transparentPbrPS, device, L"TransparentPbrPS.cso");
	initPSShader(&cubeMapPS, device, L"CubeMapPS.cso");
	initPSShader(&irradianceConvPS, device, L"IrradianceConvolutionPS.cso");
	initPSShader(&brdfPS, device, L"BrdfPS.cso");
	initPSShader(&prefilterPS, device, L"PrefilterPS.cso");
	initPSShader(&envProbePS, device, L"envProbePS.cso");
	initPSShader(&horizontalBlurPS, device, L"HorizontalBlurPS.cso");
	initPSShader(&verticalBlurPS, device, L"VerticalBlurPS.cso");
	initPSShader(&bloomLightPS, device, L"BloomLightPS.cso");
	initPSShader(&volumetricLightPS, device, L"VolumetricLightPS.cso");
	initPSShader(&postProccessPS, device, L"PostProccessPS.cso");
	initPSShader(&volumeGPassPS, device, L"VolumeGPassPS.cso");
	initPSShader(&deferredPS, device, L"DeferredPS.cso");
	initPSShader(&shadowPS, device, L"ShadowPS.cso");
	initPSShader(&shadowHorizontalBlurPS, device, L"ShadowHorizontalPS.cso");
	initPSShader(&shadowVerticalBlurPS, device, L"ShadowVerticalPS.cso");
	initPSShader(&ssaoPS, device, L"SsaoPS.cso");
	return true;
}

bool DX11::initPSShader(PixelShader* pixelShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath)
{
	if (!pixelShader->Initialize(device, filePath))
	{
		return false;
	}
	return true;
}

bool DX11::initVSShader(VertexShader* vertexShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT* numElements)
{
	if (!vertexShader->Initialize(device, filePath, layoutDesc, *numElements))
	{
		return false;
	}
	return true;
}
