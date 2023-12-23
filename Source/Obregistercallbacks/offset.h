#pragma once
#include <ntifs.h>

//============================================//
//=========== Undocumented API ===============//
//============================================//

typedef NTSTATUS(*NtQueryInformationProcess_t)(
	_In_	HANDLE					ProcessHandle,
	_Out_	PROCESSINFOCLASS		ProcessInformationClass,
	_In_	PVOID					ProcessInformation,
	_Out_	ULONG					ProcessInformationLength,
	_Out_	PULONG					ReturnLength
	);

//============================================//
//======= Structure & Global Variable ========//
//============================================//

typedef struct _IMPORT_OFFSET
{
	int			UniqueProcessid_off;
	int			ActiveProcessLinks_off;
	int			ImageFileName_off;
	int			PEB_off;
}IMPORT_OFFSET;

UCHAR* PsGetProcessImageFileName( // ���μ��� �̸� �������� �Լ�
	__in PEPROCESS Process
);
HANDLE hPid;
IMPORT_OFFSET iOffset;
const char szSystem[] = "System";
const wchar_t szNtQueryInformationProcess[] = L"NtQueryInformationProcess";