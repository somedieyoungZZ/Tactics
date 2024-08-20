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
		} while (Process32Next(pe32.szExeFile, &pe32));
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
	CloseHandle(notepad);

	return 0;
}
