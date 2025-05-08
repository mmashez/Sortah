#include <windows.h>
#include <iostream>

bool isRunningAsAdmin() {
    BOOL isElevated = FALSE;
    HANDLE token = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }

    return isElevated;
}

void runAsAdmin() {
    char fileName[MAX_PATH];
    if (GetModuleFileNameA(NULL, fileName, MAX_PATH) == 0) return;
    
    ShellExecuteA(
        NULL,
        "runas",          
        fileName,
        NULL,
        NULL,
        SW_SHOWNORMAL
    );
}

