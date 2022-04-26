#include "utils.hpp"

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

