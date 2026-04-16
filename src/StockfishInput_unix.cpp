#include "StockfishInput.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

StockfishInput::StockfishInput(const std::string& executablePath) {
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
}

StockfishInput::~StockfishInput() {
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
}

void StockfishInput::sendCommand(const std::string& command) {
    std::string fullCommand = command + "\n";
    ssize_t written = write(childStdInWrite, fullCommand.c_str(), fullCommand.size());
    if (written != static_cast<ssize_t>(fullCommand.size())) {
        throw std::runtime_error("Failed to write to Stockfish");
    }
}

std::string StockfishInput::readLine() {
    std::string line;
    char ch = 0;

    while (true) {
        ssize_t count = ::read(childStdOutRead, &ch, 1);
        if (count <= 0) {
            throw std::runtime_error("Failed to read from Stockfish");
        }

        if (ch == '\r') continue;
        if (ch == '\n') return line;

        line += ch;
    }
}
