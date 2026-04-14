#pragma once

#include <string>

class StockfishInput {
public:
    explicit StockfishInput(const std::string& executablePath);
    ~StockfishInput();

    void sendCommand(const std::string& command);
    std::string readLine();

private:
    void* childStdInWrite = nullptr;
    void* childStdOutRead = nullptr;
    void* processHandle = nullptr;
    void* threadHandle = nullptr;
};
