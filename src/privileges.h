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

void runAsAdmin(int argc, char* argv[]) {
    char fileName[MAX_PATH];
    if (GetModuleFileNameA(NULL, fileName, MAX_PATH) == 0) return;

    std::ostringstream oss;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) oss << " ";
        oss << '\"' << argv[i] << '\"';
    }

    ShellExecuteA(
        NULL,
        "runas",
        fileName,
        oss.str().c_str(),
        NULL,
        SW_SHOWNORMAL
    );
}
