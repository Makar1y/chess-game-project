#pragma once

#include <string>

#ifndef _WIN32
#include <sys/types.h>
#endif

class StockfishInput {
public:
    explicit StockfishInput(const std::string& executablePath);
    ~StockfishInput();

    void sendCommand(const std::string& command);
    std::string readLine();

private:
#ifdef _WIN32
    void* childStdInWrite = nullptr;
    void* childStdOutRead = nullptr;
    void* processHandle = nullptr;
    void* threadHandle = nullptr;
#else
    int childStdInWrite = -1;
    int childStdOutRead = -1;
    pid_t childPid = -1;
#endif
};
