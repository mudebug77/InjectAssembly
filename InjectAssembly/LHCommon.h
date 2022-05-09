#pragma once
#include <windows.h>
#include <list>
#include <string>

using namespace std;

intptr_t GetFunctionSize_(LPBYTE xBegin, LPBYTE xEnd);
//标记函数结束
#define FunctionEndFlag(x)\
int _stdcall x##_END(){ return 0x886; }

#define GetFunctionSize(x) \
	GetFunctionSize_((LPBYTE)x,(LPBYTE)x##_END)


BOOL GetProcessIdByName(LPCTSTR szProcessName, list<DWORD>& RetArray);

wstring& _cdecl Utf8ToUnicode(const char* utf8, wstring& wide);
wstring& _cdecl AsciiToUnicode(const char* ascii, wstring& wide);