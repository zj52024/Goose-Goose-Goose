#include "utils.hpp"

#include "MinHook/include/MinHook.h"
#include "kiero/kiero.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <iostream>

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

LRESULT WINAPI WndProc(HWND unnamedParam1, UINT unnamedParam2, WPARAM unnamedParam3, LPARAM unnamedParam4) {

	if (ImGui_ImplWin32_WndProcHandler(unnamedParam1, unnamedParam2, unnamedParam3, unnamedParam4)) return true;

	return CallWindowProc(oWndproc, unnamedParam1, unnamedParam2, unnamedParam3, unnamedParam4);

}

tFlip hkFlip(void* PlayerController, int someval1, int someval2, void* PhotonData) {
	//myPlayerController = PlayerController;
	//printf("HIT!   |   %p, actorNum: %d\n",PlayerController,*(int*)((long long)PlayerController+384));
	//return (tFlip)oFlip(PlayerController, someval1, someval2, PhotonData);
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


			if (ImGui::Button("Button"))
			{
				
			}


			ImGui::End();
		}

		ImGui::Render();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		canRender = !canRender;
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

