#pragma once

// ��ŷ�� LoadLibraryExw �Լ� ȣ���� ���� �ڵ鷯
HMODULE WINAPI HookLoadLibraryExw(
	IN LPCWSTR lpLibFileName,
	HANDLE hFile,
	IN DWORD dwFlags
);

// ��ũ ����
void HookFunction();

// ��ũ �и�
void UnhookFunction();

// ������ �ý��ۿ� �����ϴ��� Ȯ��
bool FileExists(const std::wstring& filePath);


// ���ڿ��� �ҹ��ڷ� ��ȯ
std::wstring ToLowerCase(const std::wstring& input);

// ���� DLL ����Ʈ
bool IsDllInAllowList(const std::wstring& filePath);