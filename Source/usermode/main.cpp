#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <iostream>
using namespace std;

bool FindProcess(char* ProcessName)
{
    TCHAR Message[50];
    HANDLE  hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);
    //프로세스가 메모리상에 있으면 첫번째 프로세스를 얻는다
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (!strcmp(pe32.szExeFile, ProcessName))
            {
                int pid = pe32.th32ProcessID;
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
                printf("%d\n", hProcess);
                if (hProcess != NULL)
                {
                    if (TerminateProcess(hProcess, 0))
                    {
                        sprintf(Message, "%s 프로세스를 종료합니다.", pe32.szExeFile);
                        MessageBox(NULL, Message, "Error", MB_OK);
                        CloseHandle(hProcess);
                        CloseHandle(hProcessSnap);
                    }
                    else
                    {
                        // Handle TerminateProcess failure if needed
                    }
                }
            }
        } while (Process32Next(hProcessSnap, &pe32)); //다음 프로세스의 정보를 구하여 있으면 루프를 돈다.
    }
    CloseHandle(hProcessSnap);
    return false;
}

int main() {
    string processname[6] = { "cheatengine-x86_64.exe","cheatengine-i386.exe","cheatengine-x86_64-SSE4-AVX2.exe", "notepad.exe", "x32dbg.exe", "x64dbg.exe"};
    while (1) {
        for (int i = 0; i < sizeof(processname); i++) {
            char* process = const_cast<char*>(processname[i].c_str());
            FindProcess(process);
        }
    }
}