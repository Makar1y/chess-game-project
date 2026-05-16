#include "Pvp.hpp"

#include <cstring>

#ifdef _WIN32
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <errno.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

bool Pvp::init() {
#ifdef _WIN32
    if (started) return true;

    WSADATA wsa;
    started = WSAStartup(MAKEWORD(2, 2), &wsa) == 0;

    return started;
#else
    started = true;
    return true;
#endif
}

void Pvp::cleanup() {
#ifdef _WIN32
    if (started) {
        WSACleanup();
        started = false;
    }
#endif
}

void Pvp::closeSocket(SocketHandle s) {
    if (s == InvalidSocket) return;

#ifdef _WIN32
    closesocket(s);
#else
    ::close(s);
#endif
}

void Pvp::nonBlocking(SocketHandle s) {
    if (s == InvalidSocket) return;

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(s, FIONBIO, &mode);
#else
    int flags = fcntl(s, F_GETFL, 0);
    if (flags != -1) {
        fcntl(s, F_SETFL, flags | O_NONBLOCK);
    }
#endif
}

Pvp::~Pvp() {
    close();
    cleanup();
}

void Pvp::close() {
    closing = true;

    closeSocket(serverSock);
    serverSock = InvalidSocket;

    closeSocket(sock);
    sock = InvalidSocket;

    buffer.clear();
}

bool Pvp::host(int port) {
    if (!init()) return false;

    close();
    closing = false;

    SocketHandle server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == InvalidSocket) return false;

    serverSock = server;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int bindResult = bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (bindResult < 0) {
        closeSocket(server);
        serverSock = InvalidSocket;
        return false;
    }

    int listenResult = listen(server, 1);
    if (listenResult < 0) {
        closeSocket(server);
        serverSock = InvalidSocket;
        return false;
    }

    SocketHandle client = InvalidSocket;

    while (!closing) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);

        timeval timeout{};
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

#ifdef _WIN32
        int ready = select(0, &readSet, nullptr, nullptr, &timeout);
#else
        int ready = select(server + 1, &readSet, nullptr, nullptr, &timeout);
#endif

        if (ready > 0 && FD_ISSET(server, &readSet)) {
            client = accept(server, nullptr, nullptr);
            break;
        }
    }

    closeSocket(server);
    serverSock = InvalidSocket;

    if (closing || client == InvalidSocket) {
        closeSocket(client);
        return false;
    }

    sock = client;
    nonBlocking(sock);

    return true;
}

bool Pvp::join(const std::string& host, int port) {
    if (!init()) return false;

    close();
    closing = false;

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    std::string portText = std::to_string(port);

    int infoResult = getaddrinfo(host.c_str(), portText.c_str(), &hints, &result);
    if (infoResult != 0 || result == nullptr) {
        return false;
    }

    SocketHandle s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (s == InvalidSocket) {
        freeaddrinfo(result);
        return false;
    }

    int connectResult = connect(s, result->ai_addr, static_cast<int>(result->ai_addrlen));

    freeaddrinfo(result);

    if (connectResult < 0) {
        closeSocket(s);
        return false;
    }

    sock = s;
    nonBlocking(sock);

    return true;
}

bool Pvp::sendLine(const std::string& text) {
    if (sock == InvalidSocket) return false;

    std::string msg = text + "\n";

    int sent = send(sock, msg.c_str(), static_cast<int>(msg.size()), 0);

    return sent > 0;
}

bool Pvp::sendMove(const std::string& move) {
    return sendLine("MOVE " + move);
}

std::string Pvp::receiveMove() {
    if (sock == InvalidSocket) return "";

    char c;

    while (true) {
        int received = recv(sock, &c, 1, 0);

        if (received > 0) {
            if (c == '\n') {
                std::string line = buffer;
                buffer.clear();
                return line;
            }

            if (c != '\r') {
                buffer += c;
            }

            continue;
        }

        if (received == 0) {
            close();
            return "";
        }

#ifdef _WIN32
        int error = WSAGetLastError();

        if (error == WSAEWOULDBLOCK) {
            return "";
        }
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return "";
        }
#endif

        close();
        return "";
    }
}
