#include <windows.h>

#include "hijack.h"
#include "hook.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		hijack_load();

		wpeh::load(hModule);
	}

	return TRUE;
}