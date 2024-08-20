#include<Windows.h>
#include<iostream>
#include<tchar.h>
#include<tlhelp32.h>

using namespace std;

int main() {

	HANDLE hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hProc == INVALID_HANDLE_VALUE) {
		cout << "Unable to create Snapshot " << GetLastError();
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	DWORD pid = 0;

	if (Process32First(hProc, &pe32)) {
		do {
			if (_tcsicmp(pe32.szExeFile, _T("notepad.exe")) == 0) {
				pid = pe32.th32ProcessID;
				cout << "Found Notepad.exe with PID " << pid << endl;
				break;
			} 
		} while (Process32Next(hProc, &pe32));
	} else {
		cout << "Unable to retrieve process information " << GetLastError() << endl;
		CloseHandle(hProc);
		return 1;
	}
	CloseHandle(hProc);

	if (pid == 0) {
		cout << "Notepad.exe is not open" << endl;
		return 1;
	}

	HANDLE notepad = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	if (notepad == NULL) {
		cout << "Failed to open process " << GetLastError() << endl;
	}
	else {
		cout << "Process Opened Successfully with Handle :" << notepad << endl;
	}
	const char* dll_path = "C:\\Users\\pjcub\\source\\repos\\HelloWorld\\x64\\Debug\\HelloWorld.dll";
	// check
	LPVOID vir_alloc = VirtualAllocEx(notepad, NULL, strlen(dll_path) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (vir_alloc == NULL) {
		cout << "Failed to allocate memory " << GetLastError() << endl;
		CloseHandle(notepad);
		return 1;
	}

	BOOL writeMem = WriteProcessMemory(notepad, vir_alloc, dll_path, strlen(dll_path) + 1, NULL);

	if (!writeMem) {
		cout << "Failed to write Process Memory" << GetLastError() << endl;
		VirtualFreeEx(notepad, vir_alloc, 0, MEM_RELEASE);
		CloseHandle(notepad);
		return 1;
	}
	LPVOID LLibary = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

	if (LLibary == NULL) {
		cout << "Failed to get adress of LoadLibary " << GetLastError() << endl;
		VirtualFreeEx(notepad, vir_alloc, 0, MEM_RELEASE);
		CloseHandle(notepad);
		return 1;
	}

	HANDLE rem_thread = CreateRemoteThread(notepad, NULL, 0, (LPTHREAD_START_ROUTINE)LLibary, vir_alloc, 0, NULL);
	if (rem_thread == NULL) {
		cout << "Failed to create remote thread " << GetLastError() << endl;
		VirtualFreeEx(notepad, vir_alloc, 0, MEM_RELEASE);
		CloseHandle(notepad);
		return 1;
	}
	else {
		cout << "Remote Thread Created" << endl;
	}

	WaitForSingleObject(notepad, INFINITE);
	VirtualFreeEx(notepad, vir_alloc, 0, MEM_RELEASE);
	CloseHandle(notepad);
	CloseHandle(rem_thread);
	return 0;
}
