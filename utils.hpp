#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>

DWORD_PTR GetGameAssemblyBase();