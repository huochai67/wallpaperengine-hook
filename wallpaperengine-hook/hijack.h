#pragma once

#include <Windows.h>
#include <Shlwapi.h>

VOID WINAPI Free();
BOOL WINAPI Load();
BOOL WINAPI Init();
FARPROC WINAPI GetAddress(PCSTR pszProcName);

bool hijack_load();