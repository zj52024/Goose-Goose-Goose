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

void* myPlayerController = nullptr; // Handlers.GameHandlers.PlayerHandlers.PlayerController
tFlip oFlip; // RVA = "0x192A290"

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

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

	return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);

}

tFlip hkFlip(void* PlayerController, int someval1, int someval2, void* PhotonData) {
	//myPlayerController = PlayerController;
	//printf("HIT!   |   %p, actorNum: %d\n",PlayerController,*(int*)((long long)PlayerController+384));
	//return (tFlip)oFlip(PlayerController, someval1, someval2, PhotonData);

	int cnt = 0;
	for (ListIterator = PlayerControllerList.begin(); ListIterator != PlayerControllerList.end(); ListIterator++) {
		if ((DWORD_PTR)PlayerController != *ListIterator) cnt++;
	}

	if (PlayerControllerList.size() == 0) { PlayerControllerList.push_back((DWORD_PTR)PlayerController); cout << "Add new: " << (DWORD_PTR)PlayerController << endl; }
	if (PlayerControllerList.size() != 0 && PlayerControllerList.size() == cnt) { PlayerControllerList.push_back((DWORD_PTR)PlayerController); cout << "Add new: " << (DWORD_PTR)PlayerController << endl; }

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

		{
			ImGui::Begin("Goose Goose Goose");                          // Create a window called "Hello, world!" and append into it.
			ImGui::Text("Hello cheater!");               // Display some text (you can use a format strings too)
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{
			ImGui::Begin("Player list");
			if (ImGui::Button("Clear")) {
				PlayerControllerList.clear();
			}
			//DWORD_PTR pRole, pNickname;
			for (ListIterator = PlayerControllerList.begin(); ListIterator != PlayerControllerList.end(); ListIterator++) {
				//pRole = *ListIterator+GooseGooseDuck::PlayerController::playerRole;

				DWORD_PTR pNickname = (*ListIterator + GooseGooseDuck::PlayerController::nickname);
				int nicknameLength = *(int*)(*(DWORD_PTR*)(pNickname)+0x10);
				wchar_t nickname[17] = L"";
				char WCTMBnickname[64] = "";

				memcpy(nickname, (DWORD_PTR*)(*(DWORD_PTR*)(pNickname)+0x14), sizeof(wchar_t) * nicknameLength);
				int len = WideCharToMultiByte(CP_UTF8, 0, nickname, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_UTF8, 0, nickname, -1, WCTMBnickname, len, NULL, NULL);

				ImGui::Text(u8"Nickname: %s\nNickname pointer: %p ", WCTMBnickname, (DWORD_PTR*)(*(DWORD_PTR*)(pNickname)+0x14));

				//Silenced: %d\nInfected: %d\nInVent: %d\nHasBomb: %d\n
				//ImGui::Text("IsGhost: %d\nIsLocal: %d", (int)(*(char*)(*ListIterator+GooseGooseDuck::PlayerController::isGhost)),(int)(*(char*)(*ListIterator+GooseGooseDuck::PlayerController::isLocal)));
				ImGui::Separator();
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
		if (AllocConsole()) {
			freopen("CONOUT$", "wb", stdout);
		}
		// define KIERO_USE_MINHOOK must be 1
		// the index of the required function can be found in the METHODSTABLE.txt
		kiero::bind(8, (void**)&oPre, hkPre);

		oFlip = (tFlip)(GetGameAssemblyBase() + 0x192A290); // 0x192A290, Flip, RVA        GameAssembly.dll+RVA

		MH_CreateHook(oFlip, hkFlip, NULL); // A big problem. without third param, it should be laggy on every flip event.
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
		setlocale(LC_ALL, "");
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

