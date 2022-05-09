#include "LHCommon.h"
#include <windows.h>
#include <Tlhelp32.h>
#include <iostream>


intptr_t GetFunctionSize_(LPBYTE xBegin, LPBYTE xEnd)
{
	intptr_t nFunctionSize = xEnd - xBegin;
	printf("GetFunctionSize_:%lld\n", nFunctionSize);
	intptr_t nRetLenth = nFunctionSize;
	for (intptr_t i = nFunctionSize - 1; i > 0; i--)
	{
		if (xBegin[i] == 0xCC)
			nRetLenth--;
		else
			break;
	}
	return nRetLenth;
}

/*************************************
* BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID)
* 功能 通过进程名获取进程PID
*
* 参数 LPSTR szProcessName 进程名
*   LPDWORD lpPID   指向保存PID的变量
* 返回是否成功
**************************************/
BOOL GetProcessIdByName(LPCTSTR szProcessName, list<DWORD>& RetArray)
{
	BOOL bRet;
	bRet = FALSE;
	RetArray.clear();
	// 变量及初始化  
	STARTUPINFO st;
	PROCESS_INFORMATION pi;
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	ZeroMemory(&st, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	st.cb = sizeof(STARTUPINFO);
	ZeroMemory(&ps, sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);
	// 遍历进程  
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (!Process32First(hSnapshot, &ps))
	{
		return FALSE;
	}
	do
	{
		// 比较进程名  
		if (lstrcmpi(ps.szExeFile, szProcessName) == 0)
		{
			RetArray.push_back(ps.th32ProcessID);
		}
	} while (Process32Next(hSnapshot, &ps));
	CloseHandle(hSnapshot);
	bRet = TRUE;
	return bRet;
}

wstring& _cdecl Utf8ToUnicode(const char* utf8, wstring& wide)
{
	const WCHAR szNULL[] = { 0x0000,0x0000 };
	wide = szNULL;

	int utf8_size;
	size_t size;
	utf8_size = strlen(utf8);
	size = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_size, NULL, 0)+1;
	wide.resize(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_size, &wide[0], size);
	return  wide;
}

wstring& _cdecl AsciiToUnicode(const char* ascii, wstring& wide)
{
	const WCHAR szNULL[] = { 0x0000,0x0000 };
	wide = szNULL;

	int ascii_size;
	int size;
	ascii_size = strlen(ascii);
	size = MultiByteToWideChar(CP_ACP, 0, ascii, ascii_size, NULL, 0)+1;

	wide.resize(size, 0);
	MultiByteToWideChar(CP_ACP, 0, ascii, ascii_size, &wide[0], size);
	return  wide;
}