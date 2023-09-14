#pragma once

#include <filesystem>

#include <Windows.h>

namespace wpeh {
	std::filesystem::path getApplicationPath() {
		TCHAR buff[MAX_PATH];
		GetModuleFileName(NULL, buff, MAX_PATH);
		return std::filesystem::path(buff);
	}
}