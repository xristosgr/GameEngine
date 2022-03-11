#include "GFXGui.h"

GFXGui::GFXGui()
{
}

void GFXGui::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);
	ImGui::StyleColorsDark();
}


void GFXGui::BeginRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::Begin("TEST");
	//
	//ImGui::End();

}

void GFXGui::EndRender()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}