/*

Crash when the game is end or quit. Need to be fixed. Very critical.
Why? Wrong PlayerController reference?

*/

#include "utils.hpp"
#include "offsets.hpp"

#include "MinHook/include/MinHook.h"
#include "kiero/kiero.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <iostream>
#include <list>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace std;

typedef HRESULT(WINAPI* PRESENT)(IDXGISwapChain*, UINT, UINT);
typedef LRESULT(WINAPI* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef void* (*tFlip)(void* PlayerController, int someval1, int someval2, void* PhotonData); // void Flip(PlayerController this, int NBPPNBKBMNF, int JMAHGLDEHHB, PhotonMessageInfo BJMKOJJELHA){}

// from imgui_demo.cpp
struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.

	ExampleAppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		ImGui::Checkbox("Auto-scroll", &AutoScroll);

		// Main window
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");

		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (clear)
			Clear();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Buf.begin();
		const char* buf_end = Buf.end();

		ImGuiListClipper clipper;
		clipper.Begin(LineOffsets.Size);
		while (clipper.Step())
		{
			for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
			{
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				ImGui::TextUnformatted(line_start, line_end);
			}
		}
		clipper.End();

		ImGui::PopStyleVar();

		if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}
};

ExampleAppLog appLog;

void* myPlayerController = nullptr; // Handlers.GameHandlers.PlayerHandlers.PlayerController
tFlip oFlip; // RVA = "0x192A290"          AOBScan?

PRESENT oPre = NULL;
WNDPROC oWndproc = NULL;

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

bool init = false;
bool canRender = true;

list<DWORD_PTR> PlayerControllerList;
list<DWORD_PTR>::iterator ListIterator;

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
		canRender = !canRender;
		return false;
	}

	if (canRender) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

		else return false;
	}
	else return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);

	/*if (canRender)
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

	if (!canRender)
		if (CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam)) return true;
		else return false;*/

}

tFlip hkFlip(void* PlayerController, int someval1, int someval2, void* PhotonData) {

	int cnt = 0;
	for (ListIterator = PlayerControllerList.begin(); ListIterator != PlayerControllerList.end(); ListIterator++) {
		if ((DWORD_PTR)PlayerController != *ListIterator) cnt++;
	}

	//if (PlayerControllerList.size() == 0) { PlayerControllerList.push_back((DWORD_PTR)PlayerController); cout << "Add new: " << (DWORD_PTR)PlayerController << endl; }
	if (PlayerControllerList.size() == 0) { PlayerControllerList.push_back((DWORD_PTR)PlayerController); appLog.AddLog("[info] Add new PlayerController: %lld\n",(DWORD_PTR)PlayerController); }
	if (PlayerControllerList.size() != 0 && PlayerControllerList.size() == cnt) { PlayerControllerList.push_back((DWORD_PTR)PlayerController); appLog.AddLog("[info] Add new PlayerController: %lld\n",(DWORD_PTR)PlayerController); }

	MH_DisableHook(oFlip);
	tFlip ret_val = (tFlip)oFlip(PlayerController, someval1, someval2, PhotonData);
	MH_EnableHook(oFlip);
	return ret_val;
}

HRESULT WINAPI hkPre(IDXGISwapChain* pSC, UINT SyncInterval, UINT Flags)
{

	//https://github.com/rdbo/ImGui-DirectX-11-Kiero-Hook/blob/master/ImGui%20DirectX%2011%20Kiero%20Hook/main.cpp
	if (!init)
	{
		if (SUCCEEDED(pSC->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSC->GetDesc(&sd);
			ID3D11Texture2D* pBackBuffer;
			pSC->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();

			oWndproc = (WNDPROC)SetWindowLongPtr(FindWindow(0, L"Goose Goose Duck"), GWLP_WNDPROC, (LONG_PTR)WndProc);

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());

			ImGui_ImplWin32_Init(FindWindow(0, L"Goose Goose Duck"));
			ImGui_ImplDX11_Init(pDevice, pContext);

			ImGui::StyleColorsDark();

			init = true;
		}
		else
			return oPre(pSC, SyncInterval, Flags);
	}

	if (canRender)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static int CurrentIdx = -1;

		{
			ImGui::Begin("Log window");
			ImGui::End();
			appLog.Draw("Log window");
		}

		{
			ImGui::Begin("Player list");
			if (ImGui::Button("Clear")) {
				PlayerControllerList.clear();
				CurrentIdx = -1;
			}

			if (ImGui::BeginListBox("PlayerControllerDataListBox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {

				for (ListIterator = PlayerControllerList.begin(); ListIterator != PlayerControllerList.end(); ListIterator++) {

					int cnt = 0;
					wchar_t nickname[17] = L"";
					char WCTMBnickname[64] = "";
					const bool is_selected = (CurrentIdx == cnt);

					DWORD_PTR pNickname = *ListIterator + GooseGooseDuck::PlayerController::nickname;
					int nicknameLength = *(int*)(*(DWORD_PTR*)(pNickname)+0x10);

					memcpy(nickname, (DWORD_PTR*)(*(DWORD_PTR*)(pNickname)+0x14), sizeof(wchar_t) * nicknameLength);
					int len = WideCharToMultiByte(CP_UTF8, 0, nickname, -1, NULL, 0, NULL, NULL);
					WideCharToMultiByte(CP_UTF8, 0, nickname, -1, WCTMBnickname, len, NULL, NULL);

					if (ImGui::Selectable(WCTMBnickname, is_selected))
						CurrentIdx = cnt;

					if (is_selected) {
						ImGui::SetItemDefaultFocus();
						appLog.AddLog(u8"[info] %lld\nNickname pointer: %p\nNickname: %s\n", *ListIterator, (DWORD_PTR*)(*(DWORD_PTR*)(pNickname)+0x14), WCTMBnickname);
						CurrentIdx = -1;
					}
					cnt++;
				}
			}
			ImGui::End();
		}

		ImGui::Render();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return oPre(pSC, SyncInterval, Flags);
}

void MainFunc(HMODULE hModule) {

	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		//if (AllocConsole()) freopen("CONOUT$", "wb", stdout);

		// define KIERO_USE_MINHOOK must be 1
		// the index of the required function can be found in the METHODSTABLE.txt
		kiero::bind(8, (void**)&oPre, hkPre);

		oFlip = (tFlip)(GetGameAssemblyBase() + GooseGooseDuck::PlayerController::flipRVA); // GameAssembly.dll+RVA

		MH_CreateHook(oFlip, hkFlip, NULL); // A big problem. without third param, it should be very very very very laggy on every flip event.
		MH_EnableHook(oFlip);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		setlocale(LC_ALL, ""); // really need?
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainFunc, hModule, 0, NULL);
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

