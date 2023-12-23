#include "common.h"


PVOID hRegistration = NULL;


BOOLEAN GetPebOffset()
{
	int LinkOffset = iOffset.ActiveProcessLinks_off;
	int ProcName = iOffset.ImageFileName_off;
	BOOLEAN success = FALSE;
	PEPROCESS Process = PsGetCurrentProcess();
	UNICODE_STRING routineName = { 0, };

	RtlInitUnicodeString(&routineName, szNtQueryInformationProcess);
	NtQueryInformationProcess_t NtQueryInformationProcess = MmGetSystemRoutineAddress(&routineName);

	for (int i = 0; i < 0x10; i++)
	{
		PROCESS_BASIC_INFORMATION ProcessInformation = { 0, };
		PLIST_ENTRY ListEntry = (PVOID*)((PCHAR)Process + LinkOffset);
		Process = ((PCHAR)ListEntry->Flink - LinkOffset);
		HANDLE Key = NULL;

		if (ObOpenObjectByPointer(Process, NULL, NULL, NULL, *PsProcessType, KernelMode, &Key) == STATUS_SUCCESS)
		{
			PULONG Ret = NULL;
			NtQueryInformationProcess(Key, ProcessBasicInformation, &ProcessInformation, sizeof(ProcessInformation), Ret);
			ZwClose(Key);
		}

		if (ProcessInformation.PebBaseAddress)
		{
			for (int j = iOffset.ActiveProcessLinks_off; j < PAGE_SIZE - 0x10; j += 4)
			{
				if (*(PHANDLE)((PCHAR)Process + j) == ProcessInformation.PebBaseAddress)
				{
					iOffset.PEB_off = j;
					success = TRUE;
					return success;
				}
			}
		}
	}
	return success;
}


BOOLEAN GetOffset(IN PEPROCESS Process)
{
	BOOLEAN success = FALSE;
	HANDLE PID = PsGetCurrentProcessId();
	PLIST_ENTRY ListEntry = { 0, };
	PLIST_ENTRY NextEntry = { 0, };

	for (int i = 0x80; i < PAGE_SIZE - 0x10; i += 4)
	{
		if (*(PHANDLE)((PCHAR)Process + i) == PID)
		{
			ListEntry = (PVOID*)((PCHAR)Process + i + 0x8);
			if (MmIsAddressValid(ListEntry) && MmIsAddressValid(ListEntry->Flink))
			{
				NextEntry = ListEntry->Flink;
				if (ListEntry == NextEntry->Blink)
				{
					iOffset.UniqueProcessid_off = i;
					iOffset.ActiveProcessLinks_off = i + 8;
					success = TRUE;
					break;
				}
			}
		}
	}
	if (!success)
	{
		DbgPrintEx(DPFLTR_ACPI_ID, 0, "[!] Not Found Offset... Sorry :(\n");
		return success;
	}

	// ImageFileName Offset 
	success = FALSE;
	for (int i = iOffset.ActiveProcessLinks_off; i < PAGE_SIZE; i++)
	{
		if (!strncmp((PCHAR)Process + i, szSystem, 6))
		{
			iOffset.ImageFileName_off = i;
			success = TRUE;
			break;
		}
	}
	if (!success)
	{
		return success;
	}

	if (!GetPebOffset())
	{
		return success;
	}
	return success;
}


NTSTATUS ObRegExample()
{
	OB_CALLBACK_REGISTRATION obRegistration = { 0, };
	OB_OPERATION_REGISTRATION opRegistration = { 0, };

	obRegistration.Version = ObGetFilterVersion();
	obRegistration.OperationRegistrationCount = 1;
	RtlInitUnicodeString(&obRegistration.Altitude, L"300000");
	obRegistration.RegistrationContext = NULL;

	opRegistration.ObjectType = PsProcessType;
	opRegistration.Operations = OB_OPERATION_HANDLE_CREATE;
	opRegistration.PreOperation = PreCallback;
	opRegistration.PostOperation = PostCallback;

	obRegistration.OperationRegistration = &opRegistration;

	DbgPrintEx(DPFLTR_ACPI_ID, 0, "[+] ObRegisterCallbacks Test\n");

	return ObRegisterCallbacks(&obRegistration, &hRegistration);
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriver, IN PUNICODE_STRING pRegPath)
{
	UNREFERENCED_PARAMETER(pRegPath);
	UNREFERENCED_PARAMETER(pDriver);

	NTSTATUS ret = STATUS_SUCCESS;
	DbgPrintEx(DPFLTR_ACPI_ID, 0, "[+] Load Driver\n");

	pDriver->DriverUnload = UnloadDriver;
	if (GetOffset(PsGetCurrentProcess()))
	{
		ret = ObRegExample();

		if (ret == STATUS_SUCCESS)
		{
			DbgPrintEx(DPFLTR_ACPI_ID, 0, "[+] Success Registeration\n");
		}
		else
		{
			DbgPrintEx(DPFLTR_ACPI_ID, 0, "[!] Failed Registration %X\n", ret);
		}
	}

	else
	{
		DbgPrintEx(DPFLTR_ACPI_ID, 0, "[!] Failed Get EPROCESS Offsets\n");
	}
	return STATUS_SUCCESS;
}

VOID UnloadDriver(IN PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);

	if (hRegistration)
	{
		ObUnRegisterCallbacks(hRegistration);
	}
	DbgPrintEx(DPFLTR_ACPI_ID, 0, "[+] Unload Driver\n");
}