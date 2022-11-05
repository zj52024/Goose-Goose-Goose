#include "esp.hpp"
#include "cheat/gameManager.hpp"

void ESPMain(list<DWORD_PTR> PlayerControllerList, playerInfo player[], DWORD_PTR LocalPlayerController, bool drawLine, bool drawBox, bool showPlayerInfo) {
	list<DWORD_PTR>::iterator ListIterator;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(1280, 720)); // 640, 360
	ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	char stateInt[5];
	int tempInt = getGameState();
	sprintf(stateInt, "%d", tempInt);
	draw_list->AddText(ImVec2(1270,695), ImColor(1.0f, 1.0f, 1.0f, 1.0f), stateInt);

	const ImU32 col = ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));   //RGBA
	int cnt = 0;
	positionXY LocalPlayerPos = { 0.0f, 0.0f };

	for (ListIterator = PlayerControllerList.begin(); ListIterator != PlayerControllerList.end(); ListIterator++) { // local-based-relative calculation needed.  linear interpolation

		if (!player[cnt].isLocal)
		{
			float deltaX, deltaY;
			memcpy(&LocalPlayerPos, (int*)(LocalPlayerController + GooseGooseDuck::PlayerController::position), 8);

			Vector3 initial = getDampFirstParam();

			deltaX = player[cnt].pos.x - LocalPlayerPos.x + initial.x;
			deltaY = player[cnt].pos.y - LocalPlayerPos.y - initial.y;

			if (drawLine)
				draw_list->AddLine(ImVec2(640 + initial.x * 80, 360 - initial.y * 80), ImVec2(640 + deltaX * 80, 360 + deltaY * -80), ImColor(0.4f, 1.0f, 0.4f, 1.0f), 2.0f);

			if (drawBox)
				draw_list->AddRect(ImVec2(600 + deltaX * 80, 310 + deltaY * -80), ImVec2(680 + deltaX * 80, 450 + deltaY * -80), ImColor(0.5f, 0.0f, 0.0f, 1.0f), 0.0f, 0, 2.0f);

			if (showPlayerInfo) {
				char infoTexts[512];
				if (!player[cnt].isGhost)
					sprintf(infoTexts, u8"\n[Player Info]\n"
						"Nickname: %s\n"
						"inVent: %s\n"
						"isInfected :%s\n"
						"isSilenced: %s\n",
						player[cnt].nickname,
						player[cnt].inVent ? "True" : "False",
						player[cnt].isInfected ? "True" : "False",
						player[cnt].isSilenced ? "True" : "False");
				draw_list->AddText(ImVec2(640 + deltaX * 80, 310 + deltaY * -80), ImColor(1.0f, 1.0f, 1.0f, 1.0f), infoTexts);
			}
		}
		cnt++;
	}
}