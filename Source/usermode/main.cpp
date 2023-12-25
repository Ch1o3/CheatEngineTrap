#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <iostream>
using namespace std;

bool GetProcessModule(DWORD dwPID, char* sProcessName);

bool FindProcess(char* ProcessName)
{
    HANDLE  hProcessSnap = NULL;
    BOOL bRet = FALSE;
    PROCESSENTRY32 pe32 = { 0 };
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == (HANDLE)-1)
        return false;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    //프로세스가 메모리상에 있으면 첫번째 프로세스를 얻는다
    if (Process32First(hProcessSnap, &pe32))
    {
        BOOL bCurrent = FALSE;
        MODULEENTRY32 me32 = { 0 };
        do
        {
            if (GetProcessModule(pe32.th32ProcessID, ProcessName) != NULL)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                if (hProcess)
                {
                    if(TerminateProcess(hProcess, 0))
                    {
                        unsigned long nCode; //프로세스 종료 상태 
                        GetExitCodeProcess(hProcess, &nCode);
                    }
                    
                    CloseHandle(hProcess);
                    return true;
                }
            }
        } while (Process32Next(hProcessSnap, &pe32)); //다음 프로세스의 정보를 구하여 있으면 루프를 돈다.
    }
    CloseHandle(hProcessSnap);
    return false;
}


bool GetProcessModule(DWORD dwPID, char* sProcessName)
{
    HANDLE hModuleSnap = NULL;
    MODULEENTRY32 me32 = { 0 };
    TCHAR process[50];

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == (HANDLE)-1)
        return (FALSE);
    me32.dwSize = sizeof(MODULEENTRY32);
    //해당 프로세스의 모듈리스트를 루프로 돌려서 프로세스이름과 동일하면 
    //true를 리턴한다.
    if (Module32First(hModuleSnap, &me32))
    {
        do
        {
            printf("process name : %s\n", me32.szModule);
            if (strcmp(me32.szModule, sProcessName) == 0)
            {
                sprintf(process, "%s 프로세스를 종료합니다.", me32.szModule);
                MessageBox(NULL, process, "Error", MB_OK);
                CloseHandle(hModuleSnap);
                return true;
            }
            //if(me32.szModule == sProcessName)
            //{ 
            //    CloseHandle (hModuleSnap); 
            //    return true;
            //} 
        } while (Module32Next(hModuleSnap, &me32));
    }
    CloseHandle(hModuleSnap);
    return false;
}


int main() {
    string processname[4] = { "Cheat Engine.exe","cheatengine-x86_64-SSE4-AVX2.exe", "x64dbg.exe", "x32dbg.exe" };
    while (1) {
        for (int i = 0; i < sizeof(processname); i++) {
            char* process = const_cast<char*>(processname[i].c_str());
            FindProcess(process);
        }
    }
}

