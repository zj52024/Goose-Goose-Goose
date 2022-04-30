#include "utils.hpp"
#include "Role.hpp"

//https://stackoverflow.com/questions/26572459/c-get-module-base-address-for-64bit-application
DWORD_PTR GetProcessBaseAddress(DWORD processID)
{
	DWORD_PTR   baseAddress = 0;
	HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	HMODULE* moduleArray;
	LPBYTE      moduleArrayBytes;
	DWORD       bytesRequired;
	wchar_t moduleName[25] = L"";

	if (processHandle)
	{
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
		{
			if (bytesRequired)
			{
				moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes)
				{
					unsigned int moduleCount;

					moduleCount = bytesRequired / sizeof(HMODULE);
					moduleArray = (HMODULE*)moduleArrayBytes;

					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
					{
						for (unsigned int i = 0; i < moduleCount; i++) {
							GetModuleBaseName(processHandle, moduleArray[i], moduleName, 24);
							if (wcscmp(moduleName, L"GameAssembly.dll") == 0) baseAddress = (DWORD_PTR)moduleArray[i];
						}
					}
					LocalFree(moduleArrayBytes);
				}
			}
		}
		CloseHandle(processHandle);
	}

	return baseAddress;
}



DWORD_PTR GetGameAssemblyBase() {
	return GetProcessBaseAddress(GetCurrentProcessId());
}

// I'm so sorry for these codes, I will modify this as soon as possible.
const char* retRole(int j) {

	switch (j)
	{

	case roles::duck::Assassin:
		return("Duck - Assassin");
		break;

	case roles::duck::Cannibal:
		return("Duck - Cannibal");
		break;

	case roles::duck::Demolitionist:
		return("Duck - Demolitionist");
		break;

	case roles::duck::Duck:
		return("Duck - Duck");
		break;

	case roles::duck::Hitman:
		return("Duck - Hitman");
		break;

	case roles::duck::Morphling:
		return("Duck - Morphling");
		break;

	case roles::duck::Party:
		return("Duck - Party");
		break;

	case roles::duck::Professional:
		return("Duck - Professional");
		break;

	case roles::duck::Silencer:
		return("Duck - Silencer");
		break;

	case roles::duck::Snitch:
		return("Duck - Snitch");
		break;

	case roles::duck::Spy:
		return("Duck - Spy");
		break;

	case roles::duck::Lover:
		return("Duck - Lover");
		break;

	case roles::goose::Mimic:
		return("Goose - Mimic");
		break;

	case roles::goose::Birdwatcher:
		return("Goose - Birdwatcher");
		break;

	case roles::goose::Bodyguard:
		return("Goose - Bodyguard");
		break;

	case roles::goose::Canadian:
		return("Goose - Canadian");
		break;

	case roles::goose::Celebrity:
		return("Goose - Celebrity");
		break;

	case roles::goose::Detective:
		return("Goose - Detective");
		break;

	case roles::goose::Goose:
		return("Goose - Goose");
		break;

	case roles::goose::Gravy:
		return("Goose - Gravy");
		break;

	case roles::goose::Locksmith:
		return("Goose - Locksmith");
		break;

	case roles::goose::Mechanic:
		return("Goose - Mechanic");
		break;

	case roles::goose::Medium:
		return("Goose - Medium");
		break;

	case roles::goose::Mortician:
		return("Goose - Mortician");
		break;

	case roles::goose::Politician:
		return("Goose - Politician");
		break;

	case roles::goose::Sherrif:
		return("Goose - Sherrif");
		break;

	case roles::goose::Snoop:
		return("Goose - Snoop");
		break;

	case roles::goose::Tech:
		return("Goose - Tech");
		break;

	case roles::goose::Vigilante:
		return("Goose - Vigilante");
		break;

	case roles::goose::Lover:
		return("Goose - Lover");
		break;

	case roles::special::Dodo:
		return("Special - Dodo");
		break;

	case roles::special::Falcon:
		return("Special - Falcon");
		break;

	case roles::special::Pigeon:
		return("Special - Pigeon");
		break;

	case roles::special::Vulture:
		return("Special - Vulture");
		break;

	case roles::special::NoRole:
		return("Special - No role");
		break;

	case roles::halloween::Thrall:
		return("Halloween - Thrall");
		break;

	case roles::halloween::Vampire:
		return("Halloween - Vampire");
		break;

	case roles::halloween::Villager:
		return("Halloween - Villager");
		break;

	default:
		return("idk");
		break;

	}

}