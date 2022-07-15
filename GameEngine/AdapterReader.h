#pragma once
#include "ErrorLogger.h"
#include <d3d11.h>

#include <wrl/client.h>
#include <vector> 


class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC description;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
	static void SetAdapterOutput(AdapterData& adapter, IDXGIOutput* adapterOutput);

private:
	static std::vector<AdapterData> adapters;
};