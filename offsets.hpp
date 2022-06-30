namespace GooseGooseDuck {
	namespace PlayerController {
		constexpr int flipRVA = 0x17381D0; // Use AOBScan
		constexpr int updateRVA = 0x174FAD0; // AOBScan too.
		constexpr int isSilenced = 0xB9; // Boolean
		constexpr int isInfected = 0xBB; // Boolean
		constexpr int playerRoleId = 0xE0; // pointer, [instance+playerRoleId] + 0x10, int
		constexpr int isPlayerRoleSet = 0xE8; // Boolean
		constexpr int inVent = 0xE9; // Boolean
		constexpr int hasBomb = 0x11B; // Boolean
		constexpr int isGhost = 0x168; // Boolean
		constexpr int isLocal = 0x198; // Boolean
		constexpr int nickname = 0x1A8; // pointer, [instance+nickname] +  0x14, length: [instance+nickname] + 0x10
		constexpr int invisibilityDistance = 0x304; // int, need this?
		constexpr int isSpectator = 0x352; // Boolean
		constexpr int isRemoteSpectating = 0x353; // Boolean
		constexpr int playerNameRoleText = 0x108; // pointer 
		constexpr int m_text = 216; // c# string   length: [[playerController+playerNameRoleText] + 216] + 0x10    string: [[playerController+playerNameRoleText] + 216] + 0x14
		constexpr int position = 0x2A0; // Value name is randomized. x, y. Float, Float
		/*
		[Notable offsets]
		2A0, Player location, Float Float,   x, y
		
		*/
	}
}