#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <Windows.h>

class GFXGui
{
public:
	GFXGui();

	void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void BeginRender();
	void EndRender();

	void EditorStyle();
};

