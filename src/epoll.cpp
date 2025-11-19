#include "epoll.h"
#include <cstdio>

Epoll::Epoll(int maxEv) : _maxEvents(maxEv), _events(maxEv){
    _epollFd = epoll_create1(0);
    if(_epollFd < 0) perror("epoll_create1");
}

Epoll::~Epoll(){
    if(_epollFd >= 0) close(_epollFd);
}

bool Epoll::add(int fd, uint32_t events, EventCallback callBack){
    epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;

    if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0){
        perror("epoll_ctl EPOLL_CTL_ADD");
        return false;
    }

    _callbacks[fd] = callBack;
    return true;
}

bool Epoll::modify(int fd, uint32_t events){
    epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;

    if(epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) < 0){
        perror("epoll_ctl EPOLL_CTL_MOD");
        return false;
    }

    return true;
}

bool Epoll::remove(int fd){
    if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, nullptr) < 0){
        perror("epoll_ctl EPOLL_CTL_DEL");
        return false;
    }

    return true;
}

void Epoll::wait(int timeoutMs){
    int n = epoll_wait(_epollFd, _events.data(), _maxEvents, timeoutMs);
    if(n < 0){
        perror("epoll_wait");
        return;
    }

    for(int i = 0; i < n; i++){
        int fd = _events[i].data.fd;
        uint32_t events = _events[i].events;
        handleEvent(fd, events);
    }
}

void Epoll::handleEvent(int fd, uint32_t events){
    auto it = _callbacks.find(fd);
    if(it != _callbacks.end()) it->second(fd, events);
}
