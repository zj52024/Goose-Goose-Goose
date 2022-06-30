#include "utils.hpp"

//https://stackoverflow.com/questions/26572459/c-get-module-base-address-for-64bit-application
DWORD_PTR GetProcessBaseAddress(DWORD processID, const wchar_t* targetModuleName)
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
							if (wcscmp(moduleName, targetModuleName) == 0) baseAddress = (DWORD_PTR)moduleArray[i];
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


void AOBScan(char* str) {

}


DWORD_PTR GetGameAssemblyBase(const wchar_t* targetModuleName) {
	return GetProcessBaseAddress(GetCurrentProcessId(), targetModuleName);
}

static Gdiplus::Pen* white_pen = new Gdiplus::Pen(Gdiplus::Color(255, 255, 255, 255), 1); //		a r g b		For goose
static Gdiplus::Pen* red_pen = new Gdiplus::Pen(Gdiplus::Color(255, 180, 0, 0), 1); //					a r g b		For duck
static Gdiplus::Pen* blue_pen = new Gdiplus::Pen(Gdiplus::Color(255, 180, 0, 0), 1); //					a r g b		For falcon and vulture
static Gdiplus::Pen* orange_pen = new Gdiplus::Pen(Gdiplus::Color(255, 180, 0, 0), 1); //				a r g b		For Neutrality except above ones

// roleType Goose: 0, duck: 1, falcon and vulture: 2, remains: 3
void drawBox(int x, int y, int width, int height, int roleType) {

}

void drawLine(int x1, int y1, int x2, int y2, int roleType) {

}