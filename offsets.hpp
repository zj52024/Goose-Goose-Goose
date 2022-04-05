#pragma once

namespace GooseGooseDuck {
	namespace PlayerController {
		constexpr unsigned long Base = 0x2E6C220; // Handlers.GameHandlers.PlayerHandlers.PlayerController
		constexpr unsigned long Nickname = 0x1A8; // string, pointer
		constexpr unsigned long RoleIdPtr = 0xE0; // ptr
		constexpr unsigned long isSilenced = 0xB9; // boolean
		constexpr unsigned long isHelium = 0xBA; // boolean
		constexpr unsigned long isInfected = 0xBB; // boolean
	}
}