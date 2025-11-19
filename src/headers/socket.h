#pragma once

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Sock
{
public:
    enum class Type {TCP, UDP};

    Sock(Type);
    virtual ~Sock();

    virtual bool sbind(const std::string&, uint16_t) = 0;
    virtual bool slisten(int);   // TCP Only
    virtual Sock* saccept();     // TCP Only
    virtual ssize_t ssend(
                        const void*, 
                        size_t, 
                        const std::string&, 
                        uint16_t) = 0;
    virtual ssize_t srecv(
                        void*, 
                        size_t, 
                        std::string&, 
                        uint16_t&) = 0;

    int getFd() const {return _sockfd;}
    Type getType() const {return _type;}

protected:
    int     _sockfd;
    Type    _type;
};

class TCPSock : public Sock
{
public:
    TCPSock();
    ~TCPSock();

    bool sbind(const std::string&, uint16_t) override;
    bool slisten(int) override;
    Sock* saccept() override;
    ssize_t ssend(
                const void*, 
                size_t, 
                const std::string&, 
                uint16_t) override;
    ssize_t srecv(
                void*, 
                size_t, 
                std::string&, 
                uint16_t&) override;
};

class UDPSock : public Sock
{
public:
    UDPSock();
    ~UDPSock();

    bool sbind(const std::string&, uint16_t) override;
    ssize_t ssend(
                const void*, 
                size_t, 
                const std::string&, 
                uint16_t) override;
    ssize_t srecv(
                void*, 
                size_t, 
                std::string&, 
                uint16_t&) override;
};