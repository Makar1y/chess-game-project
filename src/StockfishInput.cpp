#include "StockfishInput.hpp"

#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

StockfishInput::StockfishInput(const std::string& executablePath) {
#ifdef _WIN32
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
#else
    int stdinPipe[2];
    int stdoutPipe[2];

    if (pipe(stdinPipe) == -1) {
        throw std::runtime_error(std::string("Failed to create stdin pipe: ") + std::strerror(errno));
    }

    if (pipe(stdoutPipe) == -1) {
        close(stdinPipe[0]);
        close(stdinPipe[1]);
        throw std::runtime_error(std::string("Failed to create stdout pipe: ") + std::strerror(errno));
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(stdinPipe[0]);
        close(stdinPipe[1]);
        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        throw std::runtime_error(std::string("Failed to fork process: ") + std::strerror(errno));
    }

    if (pid == 0) {
        close(stdinPipe[1]);
        close(stdoutPipe[0]);

        if (dup2(stdinPipe[0], STDIN_FILENO) == -1 || dup2(stdoutPipe[1], STDOUT_FILENO) == -1 || dup2(stdoutPipe[1], STDERR_FILENO) == -1) {
            _exit(127);
        }

        close(stdinPipe[0]);
        close(stdoutPipe[1]);

        execlp(executablePath.c_str(), executablePath.c_str(), static_cast<char*>(nullptr));
        perror("Failed to exec Stockfish");
        _exit(127);
    }

    close(stdinPipe[0]);
    close(stdoutPipe[1]);

    childStdInWrite = stdinPipe[1];
    childStdOutRead = stdoutPipe[0];
    childPid = pid;
#endif
}

StockfishInput::~StockfishInput() {
#ifdef _WIN32
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
#else
    if (childStdInWrite != -1) {
        const char* quitCommand = "quit\n";
        write(childStdInWrite, quitCommand, 5);
        close(childStdInWrite);
    }

    if (childStdOutRead != -1) {
        close(childStdOutRead);
    }

    if (childPid != -1) {
        int status = 0;
        waitpid(childPid, &status, 0);
    }
#endif
}

void StockfishInput::sendCommand(const std::string& command) {
#ifdef _WIN32
    std::string fullCommand = command + "\n";
    DWORD written = 0;

    if (!WriteFile((HANDLE)childStdInWrite, fullCommand.c_str(), (DWORD)fullCommand.size(), &written, nullptr)) {
        throw std::runtime_error("Failed to write to Stockfish");
    }
#else
    std::string fullCommand = command + "\n";
    ssize_t written = write(childStdInWrite, fullCommand.c_str(), fullCommand.size());
    if (written != static_cast<ssize_t>(fullCommand.size())) {
        throw std::runtime_error("Failed to write to Stockfish");
    }
#endif
}

std::string StockfishInput::readLine() {
    std::string line;
    char ch = 0;

#ifdef _WIN32
    DWORD read = 0;
    while (true) {
        if (!ReadFile((HANDLE)childStdOutRead, &ch, 1, &read, nullptr) || read == 0) {
            throw std::runtime_error("Failed to read from Stockfish");
        }

        if (ch == '\r') continue;
        if (ch == '\n') return line;

        line += ch;
    }
#else
    while (true) {
        ssize_t count = ::read(childStdOutRead, &ch, 1);
        if (count <= 0) {
            throw std::runtime_error("Failed to read from Stockfish");
        }

        if (ch == '\r') continue;
        if (ch == '\n') return line;

        line += ch;
    }
#endif
}
