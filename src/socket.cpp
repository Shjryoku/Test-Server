#include "headers/socket.h"
#include "socket.h"

Sock::Sock(Type t) : _type(t), _sockfd(-1) {}

Sock::~Sock(){
    if(_sockfd >= 0){
        close(_sockfd);
        _sockfd = -1;
    }
}

bool Sock::slisten(int) { return false; }
Sock* Sock::saccept() { return nullptr; }


TCPSock::TCPSock() : Sock(Type::TCP){
    _sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sockfd < 0){
        perror("socket");
        return;
    }
}

TCPSock::~TCPSock(){
    if(_sockfd >= 0) close(_sockfd);
}

bool TCPSock::sbind(const std::string& ip, uint16_t port){
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
        perror("inet_pton");
        return false;
    }

    int opt = 1;
    if(setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("setsockopt");
        return -1;
    }
    
    return bind(_sockfd, (sockaddr*)&addr, sizeof(addr)) == 0;
}

bool TCPSock::slisten(int backlog){
    return listen(_sockfd, backlog) == 0;
}

Sock* TCPSock::saccept(){
    int clientFd = accept(_sockfd, nullptr, nullptr);
    if(clientFd < 0) return nullptr;

    TCPSock* client = new TCPSock();
    client->_sockfd = clientFd;
    return client;
}

ssize_t TCPSock::ssend(const void* data,
                       size_t size,
                       const std::string&,
                       uint16_t)
{
    return send(_sockfd, data, size, 0);
}

ssize_t TCPSock::srecv(void *buf,
                       size_t size,
                       std::string &,
                       uint16_t&)
{
    return recv(_sockfd, buf, size, 0);
}

UDPSock::UDPSock() : Sock(Type::UDP){
    _sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(_sockfd < 0){
        perror("socket");
        return;
    }
}

UDPSock::~UDPSock(){
    if(_sockfd >= 0) close(_sockfd);
}

bool UDPSock::sbind(const std::string& ip, uint16_t port){
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
        perror("inet_pton");
        return false;
    }

    return bind(_sockfd, (sockaddr*)&addr, sizeof(addr)) == 0;
}

ssize_t UDPSock::ssend(const void* data, size_t size, const std::string& ip, uint16_t port){
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
        perror("inet_pton");
        return false;
    }

    return sendto(_sockfd, data, size, 0, (sockaddr*)&addr, sizeof(addr));
}

ssize_t UDPSock::srecv(void* buf, size_t size, std::string& outIp, uint16_t& outPort){
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);

    ssize_t n = recvfrom(_sockfd, buf, size, 0, (sockaddr*)&addr, &len);
    
    if(n < 0){
        perror("recvfrom");
        return -1;
    }

    char ip[INET_ADDRSTRLEN];
    if(!inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip))) strcpy(ip, "0.0.0.0");

    outIp = ip;
    outPort = ntohs(addr.sin_port);

    return n;
}
