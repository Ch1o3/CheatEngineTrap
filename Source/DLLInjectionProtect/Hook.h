#pragma once

// 후킹한 LoadLibraryExw 함수 호출을 위한 핸들러
HMODULE WINAPI HookLoadLibraryExw(
	IN LPCWSTR lpLibFileName,
	HANDLE hFile,
	IN DWORD dwFlags
);

// 후크 부착
void HookFunction();

// 후크 분리
void UnhookFunction();

// 파일이 시스템에 존재하는지 확인
bool FileExists(const std::wstring& filePath);


// 문자열을 소문자로 변환
std::wstring ToLowerCase(const std::wstring& input);

// 허용된 DLL 리스트
bool IsDllInAllowList(const std::wstring& filePath);