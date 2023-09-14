#include <iostream>
#include <filesystem>

#include <Windows.h>
#include <TlHelp32.h>

int GetProcessPath(const DWORD pid, wchar_t* buf)
{
	auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess)
		return 0;
	DWORD size = MAX_PATH;
	QueryFullProcessImageName(hProcess, 0, buf, &size);
	CloseHandle(hProcess);
	return size;
}

DWORD GetProcessPID(const wchar_t* exe)
{
	/* 打开遍历进程句柄 */
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snap == INVALID_HANDLE_VALUE) 
		return 0;

	/* 初始化进程信息结构 */
	PROCESSENTRY32 ProcessInfo{ 0 };
	ProcessInfo.dwSize = sizeof(ProcessInfo);

	/* 循环查找进程信息 */
	if (Process32First(Snap, &ProcessInfo))
	{
		do
		{
			if (wcscmp(exe, ProcessInfo.szExeFile) == 0)
			{
				CloseHandle(Snap);
				return ProcessInfo.th32ProcessID;
			}
		} while (Process32Next(Snap, &ProcessInfo));
	}

	CloseHandle(Snap);
	return 0;
}

void patch(const DWORD pid)
{
	wchar_t buf[MAX_PATH];
	auto pathsize = GetProcessPath(pid, buf);
	if (!pathsize)
		return;

	std::filesystem::path path(buf);
	auto parent_path = path.parent_path();
	auto dllpath = parent_path;
	dllpath += "/bin/version.dll";
	if(std::filesystem::exists(dllpath))
		return;

	std::filesystem::copy_file("C:\\wppatch\\version.dll", dllpath);

	auto hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	TerminateProcess(hProcess, 0);
	//ShellExecute(NULL, L"open", buf, NULL, NULL, SW_SHOWDEFAULT);
	return;
}

int main()
{
	while (true)
	{
		if (auto pid = GetProcessPID(L"wallpaper32.exe"); pid > 0)
			patch(pid);
		if (auto pid = GetProcessPID(L"wallpaper64.exe"); pid > 0)
			patch(pid);
		Sleep(10000);
	}

	return 0;
}