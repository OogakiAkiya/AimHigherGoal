#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"../VisualCWrapper/VisualCWrapper.h"
#include "ImguiWrapper.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


ImguiWrapper::ImguiWrapper(HWND _hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(_hwnd);
	ImGui_ImplDX9_Init(DX.GetDevice());

	ImGui::StyleColorsLight();
}

ImguiWrapper::~ImguiWrapper()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImguiWrapper::UpdataStart()
{
	//äeéÌê›íËèàóù
	ImGui_ImplWin32_WndProcHandler(*WIN.GetHwnd(), WIN.GetUintMsg(), WIN.GetwParam(), WIN.GetlParam());

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void ImguiWrapper::UpdataEnd()
{
	ImGui::End();
	ImGui::EndFrame();
}

bool* ImguiWrapper::GetApperFlg()
{
	return &appearFlg;
}

void ImguiWrapper::SetAppearFlg(bool _flg)
{
	appearFlg = _flg;
}

void ImguiWrapper::Draw()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

