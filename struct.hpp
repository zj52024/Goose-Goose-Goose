#include "imgui/imgui.h"

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

struct playerInfo {
	DWORD_PTR ptrPlayerController = 0;
	bool isSilenced = false;
	bool isInfected = false;
	int playerRole = -1;
	bool isPlayerRoleSet = false;
	bool inVent = false;
	bool hasBomb = false;
	bool isGhost = false;
	bool isLocal = false;
	int invisibilityDistance = 0;
	bool isSpectator = false;
	bool isRemoteSpectating = false;
	char nickname[17] = "";

	void reset() {
		ptrPlayerController = 0;
		isSilenced = false;
		isInfected = false;
		playerRole = -1;
		isPlayerRoleSet = false;
		inVent = false;
		hasBomb = false;
		isGhost = false;
		isLocal = false;
		invisibilityDistance = 0;
		isSpectator = false;
		isRemoteSpectating = false;
		nickname[17] = {};
	}

	void update(DWORD_PTR PlayerController) {
		if (ptrPlayerController != PlayerController) {
			ptrPlayerController = PlayerController;
			wchar_t tmpNick[17] = L"";

			memcpy(tmpNick,
				(DWORD_PTR*)(*(DWORD_PTR*)(PlayerController + GooseGooseDuck::PlayerController::nickname) + 0x14),
				sizeof(wchar_t) * *(int*)(*(DWORD_PTR*)(PlayerController + GooseGooseDuck::PlayerController::nickname) + 0x10));

			int len = WideCharToMultiByte(CP_UTF8, 0, tmpNick, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, tmpNick, -1, nickname, len, NULL, NULL);

#define GET_BOOL_VALUE(X) *(bool*)(PlayerController+X)
#define GET_INT_VALUE(X) *(int*)(PlayerController+X)

			isPlayerRoleSet = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isPlayerRoleSet);

			if (isPlayerRoleSet) {
				isSilenced = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isSilenced);
				isInfected = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isInfected);
				playerRole = *(int*)(*(DWORD_PTR*)(PlayerController + GooseGooseDuck::PlayerController::playerRole) + 0x10);
				inVent = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::inVent);
				hasBomb = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::hasBomb);
				isGhost = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isGhost);
				isLocal = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isLocal);
				isSpectator = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isSpectator);
				invisibilityDistance = GET_INT_VALUE(GooseGooseDuck::PlayerController::invisibilityDistance);
				isRemoteSpectating = GET_BOOL_VALUE(GooseGooseDuck::PlayerController::isRemoteSpectating);
			}
#undef GET_BOOL_VALUE
#undef GET_INT_VALUE
		}
	}
};