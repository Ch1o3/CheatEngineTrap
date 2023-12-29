#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <iostream>

using namespace std;

// 스레드 함수
DWORD WINAPI MessageBoxThread(LPVOID lpParam)
{
    char* message = static_cast<char*>(lpParam);
    MessageBox(NULL, message, "Message", MB_OK | MB_SERVICE_NOTIFICATION);
    // 동적 할당된 메모리 해제
    delete[] message;

    return 0;
}

bool FindProcess(char* ProcessName)
{
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (strstr(pe32.szExeFile, ProcessName))
            {
                int pid = pe32.th32ProcessID;
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);


                if (hProcess != NULL)
                {
                    if (TerminateProcess(hProcess, 0))
                    {
                        printf("%s 프로세스를 종료합니다. PID = %d\n", pe32.szExeFile, pid);

                        // 동적으로 메모리 할당하여 Message 변수 생성
                        char* message = new char[60];
                        sprintf(message, "%s 프로세스를 종료합니다.", pe32.szExeFile);

                        // 메시지 박스 스레드 생성
                        CreateThread(NULL, 0, MessageBoxThread, (LPVOID)message, 0, NULL);

                        CloseHandle(hProcess);
                    }
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return false;
}

void print_menu() {
    cout << "=============================================" << endl;
    cout << "[1] Process Detection" << endl;
    cout << "[2] DLL Inject Detection" << endl;
    cout << "[3] exit" << endl;
}

int main() {
    string processname[2] = { "cheatengine", "dbg" };

    while (1) {
        for (int i = 0; i < (sizeof(processname) / sizeof(processname[0])); i++)
        {
            char* process = const_cast<char*>(processname[i].c_str());
            FindProcess(process);
        }
    }

    return 0;
}

/*#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <cstring>

using namespace std;

// 비모달 대화상자를 띄우는 함수
DWORD WINAPI ShowMessageBox(LPVOID lpParam)
{
    char* message = (char*)lpParam;
    printf("%s", message);
    MessageBoxA(NULL, message, "Error", MB_OK | MB_SERVICE_NOTIFICATION);
    return 0;
}

bool FindProcess(char* ProcessName)
{
    char Message[100];
    HANDLE  hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 프로세스가 메모리상에 있으면 첫번째 프로세스를 얻는다
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (strstr(pe32.szExeFile, ProcessName))
            {
                int pid = pe32.th32ProcessID;
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

                if (hProcess != NULL)
                {
                    if (TerminateProcess(hProcess, 0))
                    {
                        printf("%s 프로세스를 종료합니다. PID = %d\n", pe32.szExeFile, pid);


                        sprintf(Message, "%s 프로세스를 종료합니다.", pe32.szExeFile);
                        // 백그라운드 스레드에서 비모달 대화상자를 띄우기
                        CreateThread(NULL, 0, ShowMessageBox, (LPVOID)Message, 0, NULL);

                        CloseHandle(hProcess);
                        CloseHandle(hProcessSnap);
                    }
                }
            }
        } while (Process32Next(hProcessSnap, &pe32)); // 다음 프로세스의 정보를 구하여 있으면 루프를 돈다.
    }

    CloseHandle(hProcessSnap);
    return false;
}

void print_menu() {
    cout << "=============================================" << endl;
    cout << "[1] Process Detection" << endl;
    cout << "[2] DLL Detection" << endl;
    cout << "" << endl;
}

int main() {
    string processname[2] = { "cheatengine", "dbg" };

    while (1) {
        for (int i = 0; i < (sizeof(processname) / sizeof(processname[0])); i++)
        {
            char* process = const_cast<char*>(processname[i].c_str());
            FindProcess(process);
        }
    }

    return 0;
}
*/