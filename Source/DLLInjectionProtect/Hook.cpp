#include "pch.h"
#include "Hook.h"

typedef HMODULE(WINAPI* _HookLoadLibraryExW)(
	IN LPCWSTR lpLibFileName,
	HANDLE hFile,
	IN DWORD dwFlags
	);

_HookLoadLibraryExW TrueLoadLibraryExW = reinterpret_cast<_HookLoadLibraryExW>(
	GetProcAddress(GetModuleHandleW(L"kernelbase"), "LoadLibraryExW")
	);


HMODULE __stdcall HookLoadLibraryExw(IN LPCWSTR lpLibFileName, HANDLE hFile, IN DWORD dwFlags) {
	const std::wstring libName(lpLibFileName);
	// 허용된 DLL 이라면 DLL 인젝션 허용
	if (!FileExists(libName) || IsDllInAllowList(libName)) {
		return TrueLoadLibraryExW(lpLibFileName, hFile, dwFlags);
	}

	// 허용되지 않은 DLL 인젝션시 탐지
	fflush(stdout);
	std::wcout << L"Something is trying to inject DLL: " << libName.c_str() << std::endl;
	std::wcout << L"Rejected LoadLibrary function call! Application is protected from DLL injection!" << std::endl;
	MessageBox(NULL, lpLibFileName, L"Warning", MB_OK);
	SetLastError(ERROR_ACCESS_DENIED);
	return NULL;
}

void HookFunction() {
	DetourRestoreAfterWith();
	fflush(stdout);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(reinterpret_cast<PVOID&>(TrueLoadLibraryExW)), HookLoadLibraryExw);
	const LONG error = DetourTransactionCommit();

	if (error == NO_ERROR) {
		std::wcout << L"Detoured LoadLibraryExW()." << std::endl;
	}
	else {
		std::wcout << L"Error detouring LoadLibraryExW():" << error << std::endl;
	}
}

void UnhookFunction() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(reinterpret_cast<PVOID&>(TrueLoadLibraryExW)), HookLoadLibraryExw);
	const LONG error = DetourTransactionCommit();

	std::wcout << L"Removed LoadLibraryExW(). Result = " << error << std::endl;
	fflush(stdout);
}

bool FileExists(const std::wstring& filePath) {
	const DWORD fileAttributes = GetFileAttributesW(filePath.c_str());

	return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring ToLowerCase(const std::wstring& input) {
	std::wstring lowerCaseString;
	std::locale loc;

	for (std::wstring::size_type i = 0; i < input.length(); ++i) {
		lowerCaseString += std::tolower(input[i], loc);
	}

	return lowerCaseString;
}

bool IsDllInAllowList(const std::wstring& filePath) {
	static const std::vector<std::wstring> allowdFolders =
	{
		L"c:\\windows\\system32",
		L"c:\\windows\\syswow64",       // For 32-bit dlls on 64-bit systems.
		L"c:\\windows\\microsoft.net",  // For .net framework and core folder.
		L"c:\\windows\\assembly",       // For common libraries.
		L"c:\\program files",
		L"c:\\program files (x86)",     // For 32-bit applications on 64-bit systems
		L"c:\\programdata",
		L"c:\\users\\public"
	};

	const std::wstring dllPath = ToLowerCase(filePath);
	for (const auto& allowedFolder : allowdFolders) {
		if (dllPath.find(allowedFolder) != std::wstring::npos) {
			return true;
		}
	}
	return false;
}