#include <windows.h>
#include <iostream>
#include <string>

bool is_prime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; ++d) {
        if (n % d == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc == 4 && std::string(argv[1]) == "--child") {
        HANDLE read_fd = (HANDLE)(ULONG_PTR)std::stoull(argv[2]);
        HANDLE write_fd = (HANDLE)(ULONG_PTR)std::stoull(argv[3]);

        int range[2];
        DWORD bytesRead, bytesWritten;

        if (ReadFile(read_fd, range, sizeof(range), &bytesRead, NULL) && bytesRead == sizeof(range)) {
            int start = range[0];
            int end = range[1];

            for (int val = start; val <= end; ++val) {
                if (is_prime(val)) {
                    WriteFile(write_fd, &val, sizeof(int), &bytesWritten, NULL);
                }
            }
        }

        CloseHandle(read_fd);
        CloseHandle(write_fd);
        return 0;
    }

    const int NUM_PROCS = 10;
    const int STEP = 1000;

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE p2c[NUM_PROCS][2];
    HANDLE c2p[NUM_PROCS][2];
    PROCESS_INFORMATION pi[NUM_PROCS];

    for (int i = 0; i < NUM_PROCS; ++i) {
        CreatePipe(&p2c[i][0], &p2c[i][1], &sa, 0);
        SetHandleInformation(p2c[i][1], HANDLE_FLAG_INHERIT, 0);

        CreatePipe(&c2p[i][0], &c2p[i][1], &sa, 0);
        SetHandleInformation(c2p[i][0], HANDLE_FLAG_INHERIT, 0);

        std::string cmd = std::string("\"") + exePath + "\" --child " +
            std::to_string((ULONG_PTR)p2c[i][0]) + " " +
            std::to_string((ULONG_PTR)c2p[i][1]);

        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi[i], sizeof(pi[i]));

        CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi[i]);

        CloseHandle(p2c[i][0]);
        CloseHandle(c2p[i][1]);

        int range[2] = { i * STEP + 1, (i + 1) * STEP };
        DWORD bytesWritten;
        WriteFile(p2c[i][1], range, sizeof(range), &bytesWritten, NULL);
        CloseHandle(p2c[i][1]);
    }

    std::cout << "Numere prime pana la 10000:\n";
    for (int i = 0; i < NUM_PROCS; ++i) {
        int prime;
        DWORD bytesRead;
        while (ReadFile(c2p[i][0], &prime, sizeof(int), &bytesRead, NULL) && bytesRead > 0) {
            std::cout << prime << " ";
        }
        CloseHandle(c2p[i][0]);
    }
    std::cout << "\n";

    for (int i = 0; i < NUM_PROCS; ++i) {
        WaitForSingleObject(pi[i].hProcess, INFINITE);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    return 0;
}