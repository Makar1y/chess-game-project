#include "StockfishInput.hpp"

#include <stdexcept>
#include <windows.h>

StockfishInput::StockfishInput(const std::string& executablePath) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;

    HANDLE childStdOutWrite = nullptr;
    HANDLE childStdInRead = nullptr;

    if (!CreatePipe((PHANDLE)&childStdOutRead, &childStdOutWrite, &sa, 0)) {
        throw std::runtime_error("Failed to create stdout pipe");
    }

    if (!SetHandleInformation((HANDLE)childStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
        throw std::runtime_error("Failed to configure stdout pipe");
    }

    if (!CreatePipe(&childStdInRead, (PHANDLE)&childStdInWrite, &sa, 0)) {
        throw std::runtime_error("Failed to create stdin pipe");
    }

    if (!SetHandleInformation((HANDLE)childStdInWrite, HANDLE_FLAG_INHERIT, 0)) {
        throw std::runtime_error("Failed to configure stdin pipe");
    }

    STARTUPINFOA si{};
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = childStdInRead;
    si.hStdOutput = childStdOutWrite;
    si.hStdError = childStdOutWrite;

    PROCESS_INFORMATION pi{};
    std::string commandLine = "\"" + executablePath + "\"";

    if (!CreateProcessA(
            nullptr,
            commandLine.data(),
            nullptr,
            nullptr,
            TRUE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &si,
            &pi)) {
        throw std::runtime_error("Failed to start Stockfish");
    }

    CloseHandle(childStdInRead);
    CloseHandle(childStdOutWrite);

    processHandle = pi.hProcess;
    threadHandle = pi.hThread;
}

StockfishInput::~StockfishInput() {
    if (childStdInWrite != nullptr) {
        DWORD written = 0;
        const char* quitCommand = "quit\n";
        WriteFile((HANDLE)childStdInWrite, quitCommand, 5, &written, nullptr);
        CloseHandle((HANDLE)childStdInWrite);
    }

    if (childStdOutRead != nullptr) {
        CloseHandle((HANDLE)childStdOutRead);
    }

    if (threadHandle != nullptr) {
        CloseHandle((HANDLE)threadHandle);
    }

    if (processHandle != nullptr) {
        CloseHandle((HANDLE)processHandle);
    }
}

void StockfishInput::sendCommand(const std::string& command) {
    std::string fullCommand = command + "\n";
    DWORD written = 0;

    if (!WriteFile((HANDLE)childStdInWrite, fullCommand.c_str(), (DWORD)fullCommand.size(), &written, nullptr)) {
        throw std::runtime_error("Failed to write to Stockfish");
    }
}

std::string StockfishInput::readLine() {
    std::string line;
    char ch = 0;
    DWORD read = 0;

    while (true) {
        if (!ReadFile((HANDLE)childStdOutRead, &ch, 1, &read, nullptr) || read == 0) {
            throw std::runtime_error("Failed to read from Stockfish");
        }

        if (ch == '\r') continue;
        if (ch == '\n') return line;

        line += ch;
    }
}
