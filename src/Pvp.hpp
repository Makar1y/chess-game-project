#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#ifdef _WIN32
    using SocketHandle = std::uintptr_t;
    constexpr SocketHandle InvalidSocket =
        static_cast<SocketHandle>(~static_cast<SocketHandle>(0));
#else
    using SocketHandle = int;
    constexpr SocketHandle InvalidSocket = -1;
#endif

class Pvp {
private:
    SocketHandle sock = InvalidSocket;
    SocketHandle serverSock = InvalidSocket;

    bool started = false;
    std::atomic_bool closing{ false };
    std::string buffer;

    bool init();
    void cleanup();

    void closeSocket(SocketHandle s);
    void nonBlocking(SocketHandle s);

public:
    ~Pvp();

    bool host(int port = 54000);
    bool join(const std::string& host, int port = 54000);

    bool sendLine(const std::string& text);
    bool sendMove(const std::string& move);

    std::string receiveMove();

    void close();
};
