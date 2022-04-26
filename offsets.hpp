namespace GooseGooseDuck {
	namespace PlayerController {
		constexpr int flipRVA = 0x16BD070; // Use AOBScan
		constexpr int isSilenced = 185; // Boolean
		constexpr int isInfected = 187; // Boolean
		constexpr int playerRole = 224; // pointer, [instance+playerRole] + 0x10, int
		constexpr int isPlayerRoleSet = 232; // Boolean
		constexpr int inVent = 233; // Boolean
		constexpr int hasBomb = 282; // Boolean
		constexpr int isGhost = 360; // Boolean
		constexpr int isLocal = 408; // Boolean
		constexpr int nickname = 424; // pointer, [instance+nickname] +  0x14, length: [instance+nickname] + 0x10
		constexpr int invisibilityDistance = 740; // int, need this?
		constexpr int isSpectator = 818; // Boolean
		constexpr int isRemoteSpectating = 819; // Boolean
	}
}