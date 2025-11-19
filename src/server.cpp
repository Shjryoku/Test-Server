#include "server.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

Server::Server(const std::string& ip, uint16_t port, bool useUDP) : 
                                                                    _ip(ip), _port(port), _useUdp(useUDP), _tcpFd(nullptr), 
                                                                    _udpFd(nullptr), _epoll(1024), _running(false) {}

Server::~Server(){
    stop();
    if(_tcpFd) { close(_tcpFd->getFd()); delete _tcpFd; _tcpFd = nullptr; }
    if(_udpFd) { close(_udpFd->getFd()); delete _udpFd; _udpFd = nullptr; }
}

bool Server::start(){
    if(!createSock()) return false;
    _running = true;
    return true;
}

bool Server::stop(){
    _running = false;
    return true;
}

bool Server::run(){
    while(_running) _epoll.wait(1000);
    return true;
}

ssize_t Server::sendTo(int fd, const void* data, size_t size){
    return send(fd, data, size, 0);
}

ssize_t Server::sendUdp(const sockaddr_in& addr, const void* data, size_t size){
    if(!_udpFd) return -1;
    return sendto(_udpFd->getFd(), data, size, 0, (sockaddr*)&addr, sizeof(addr));
}

void Server::setOnClientConnected(OnClientConnected oCC) { _oCC = oCC; }
void Server::setOnClientDisconnected(OnClientDisconnected oCD) { _oCD = oCD; }
void Server::setOnDataReceived(OnDataReceived oDR) { _oDR = oDR; }

bool Server::createSock(){
    if(_useUdp){
        _udpFd = new UDPSock();
        if(!_udpFd) return false;

        if(!_udpFd->sbind(_ip, _port)){
            delete _udpFd;
            _udpFd = nullptr;
            return false;
        }

        if(!configNonBlocking(_udpFd->getFd())) return false;

        _epoll.add(_udpFd->getFd(), EPOLLIN, [this](int fd, uint32_t){
            handleClientData(fd, EPOLLIN);
        });
    } else {
        _tcpFd = new TCPSock();
        if(!_tcpFd) return false;

        if(!_tcpFd->sbind(_ip, _port) || !_tcpFd->slisten(128)){
            delete _tcpFd;
            _tcpFd = nullptr;
            return false;
        }

        if(!configNonBlocking(_tcpFd->getFd())) return false;

        _epoll.add(_tcpFd->getFd(), EPOLLIN, [this](int fd, uint32_t){
            handleNewConnection(fd, EPOLLIN);
        });
    }

    return true;
}

bool Server::configNonBlocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    if( flags < 0) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

void Server::handleNewConnection(int, uint32_t){
    if(!_tcpFd) return;

    TCPSock* client = static_cast<TCPSock*>(_tcpFd->saccept());
    if(!client) return;

    int clientFd = client->getFd();

    configNonBlocking(clientFd);

    _epoll.add(clientFd, EPOLLIN, [this](int fd, uint32_t ev){
        handleClientData(fd, ev);
    });

    if(_oCC){
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);
        getpeername(clientFd, (sockaddr*)&addr, &len);
        _clients.insert(clientFd);
        _stats.curr_clients++;
        _stats.total_uniq_clients++;
        _oCC(clientFd, addr);
    }
}

void Server::handleClientData(int fd, uint32_t){
    char buff[4096];

    if(_useUdp && _udpFd && fd == _udpFd->getFd()){
        std::string ip;
        uint16_t port;
        ssize_t n = _udpFd->srecv(buff, sizeof(buff), ip, port);

        if(n > 0 && _oDR){
            _oDR(fd, buff, n);
        }
        
        -_udpFd->ssend(buff, n, ip, port);

        return;
    }
    ssize_t n = recv(fd, buff, sizeof(buff), 0);
    if(n <= 0){
        _epoll.remove(fd);
        if(_oCD) _oCD(fd);
        _clients.erase(fd);
        _stats.curr_clients--;
        close(fd);
        return;
    }

    if(_oDR) _oDR(fd, buff, n);
}

const ServerStats& Server::getStats() const {
    return this->_stats;
}