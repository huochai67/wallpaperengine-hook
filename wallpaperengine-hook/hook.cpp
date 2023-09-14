#include <filesystem>
#include <format>
#include <chrono>
#include <iostream>
#include <wtypes.h>

#include <MinHook.h>

#include "hook.hpp"
#include "utils.hpp"

namespace {
    inline void Log(const std::wstring& str)
    {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        std::wcout << std::format(L"[{:%Y-%m-%d %X}] {}", time, str) << std::endl;
    }
    template<typename T>
    inline void LogAndExit(T arg)
    {
        Log(arg);
        wpeh::Exit();
    }
}

bool disable = false;
HMODULE wpeh_hmodule;

typedef HANDLE(*CREATEFILEW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
CREATEFILEW fpCreateFileW = nullptr;

HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    if(disable)
        return fpCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    static const wchar_t* filename = L"project.json";
    static const size_t filenamesize = wcslen(filename);
    static const wchar_t* patchpath = L"c:\\wppatch\\project.json";

    bool patch = false;
    auto strsize = wcslen(lpFileName);
    if (strsize < filenamesize)
        patch = false;
    if (wcscmp(lpFileName + strsize - filenamesize, filename) == 0)
        patch = true;
    if (patch)
    {
        Log(std::format(L"redirect: {} to {}", lpFileName, patchpath));
        return fpCreateFileW(patchpath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    return fpCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
};

void wpeh::Exit()
{
    FreeLibraryAndExitThread(wpeh_hmodule, 0);
}

void wpeh::load(void* hm)
{
#if _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    SetConsoleTitleW(L"WallpaperEngine hook");
    SetConsoleOutputCP(CP_UTF8);

    Log(L"Debug mode.");
#endif

    if(std::filesystem::exists("c://disablehook.txt"))
        disable = true;

    wpeh_hmodule = (HMODULE)hm;

    {
        auto ret = MH_Initialize();
        if (ret != MH_OK)
            LogAndExit(std::format(L"error {}.", (int)ret));
    }

    {
        auto ret = MH_CreateHook(&CreateFileW, &MyCreateFileW, reinterpret_cast<void**>(&fpCreateFileW));
        if (ret != MH_OK)
            LogAndExit(std::format(L"error {}.", (int)ret));
    }
    {
        auto ret = MH_EnableHook(&CreateFileW);
        if (ret != MH_OK)
            LogAndExit(std::format(L"error {}.", (int)ret));
    }

    Log(L"Dll has injected!");
}