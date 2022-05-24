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

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiBackendFlags_PlatformHasViewports;
	//io.ConfigFlags |= ImGuiBackendFlags_RendererHasViewports;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);
	ImGui::StyleColorsDark();
}


void GFXGui::BeginRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void GFXGui::EndRender()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}