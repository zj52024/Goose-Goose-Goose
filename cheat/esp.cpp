#include "esp.hpp"
#include "cheat/gameManager.hpp"
#include "cheat/unityEngineCamera.hpp"
#include "utils.hpp"

// Only works if getGameState() == gameStateCode::InGame
void ESPMain(list<DWORD_PTR> PlayerControllerList, playerInfo player[], DWORD_PTR LocalPlayerController, bool drawLine, bool drawBox, bool showPlayerInfo) {
	if (isCameraInstanceSet()) {
		list<DWORD_PTR>::iterator ListIterator;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(1280, 720)); // 640, 360
		ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		char stateInt[5];
		int tempInt = getGameState();
		sprintf(stateInt, "%d", tempInt);
		draw_list->AddText(ImVec2(1270, 695), ImColor(1.0f, 1.0f, 1.0f, 1.0f), stateInt);

		const ImU32 col = ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));   //RGBA

		int cnt = 0;
		Vector3 LocalPlayerPos = WorldToScreenPoint(player[0].pos);

		for (ListIterator = PlayerControllerList.begin(); ListIterator != PlayerControllerList.end(); ListIterator++) { // local-based-relative calculation needed.  linear interpolation

			if (!player[cnt].isLocal)
			{
				Vector3 targetPos = WorldToScreenPoint(player[cnt].pos);

				if (drawLine)
					draw_list->AddLine(ImVec2(LocalPlayerPos.x, 720.0f-LocalPlayerPos.y), ImVec2(targetPos.x, 720.0f-targetPos.y), ImColor(0.4f, 1.0f, 0.4f, 1.0f), 2.0f);

				if (drawBox)
					draw_list->AddRect(ImVec2(targetPos.x - 40.0f, 720.0f-(targetPos.y - 70.0f)), ImVec2(targetPos.x + 40.0f, 720.0f-(targetPos.y + 70.0f)), ImColor(0.5f, 0.0f, 0.0f, 1.0f), 0.0f, 0, 2.0f);

				if (showPlayerInfo) {
					char infoTexts[512];
					if (!player[cnt].isGhost)
						sprintf(infoTexts, u8"\n[角色信息]\n"
							u8"游戏ID: %s\n"
							u8"身份: %s\n"
							u8"通风管?: %s\n"
							u8"是否被感染 :%s\n"
							u8"静音状态: %s\n",
							player[cnt].nickname,
							returnRoleName(player[cnt].playerRoleId),
							player[cnt].inVent ? "True" : "False",
							player[cnt].isInfected ? "True" : "False",
							player[cnt].isSilenced ? "True" : "False");
					draw_list->AddText(ImVec2(targetPos.x + 50.0f, 720.0f-(targetPos.y - 40.0f)), ImColor(1.0f, 1.0f, 1.0f, 1.0f), infoTexts);
				}
			}
			cnt++;
		}
	}
}
