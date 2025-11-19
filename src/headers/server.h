#pragma once
#include "socket.h"
#include "epoll.h"
#include <unordered_set>
#include "commands.h"

class Server{
public:
    using OnClientConnected     = std::function<void(int, sockaddr_in)>;
    using OnClientDisconnected  = std::function<void(int)>;
    using OnDataReceived        = std::function<void(int, const char*, size_t)>;

    Server(const std::string&, uint16_t, bool);
    ~Server();

    bool start();
    bool stop();
    bool run();

    ssize_t sendTo(int, const void*, size_t);
    ssize_t sendUdp(const sockaddr_in&, const void*, size_t);

    void setOnClientConnected(OnClientConnected);
    void setOnClientDisconnected(OnClientDisconnected);
    void setOnDataReceived(OnDataReceived);

    const ServerStats& getStats() const;
private:
    bool createSock();
    bool configNonBlocking(int);
    void handleNewConnection(int, uint32_t);
    void handleClientData(int, uint32_t);

    std::string _ip;
    uint16_t    _port;

    TCPSock*    _tcpFd;
    UDPSock*    _udpFd;

    bool        _useUdp;

    Epoll       _epoll;

    OnClientConnected       _oCC;
    OnClientDisconnected    _oCD;
    OnDataReceived          _oDR;

    std::unordered_set<int> _clients;

    bool        _running;

    ServerStats _stats;
};