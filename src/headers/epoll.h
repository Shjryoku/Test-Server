#pragma once

#include <unistd.h>
#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>

class Epoll{
public:
    using EventCallback = std::function<void(int, uint32_t)>;

    Epoll(int);
    ~Epoll();

    bool add(int, uint32_t, EventCallback);
    bool modify(int, uint32_t);
    bool remove(int);
    void wait(int);

    void getEvent();
private:
    int     _epollFd;
    int     _maxEvents;

    std::vector<struct epoll_event> _events;
    std::unordered_map<int, EventCallback> _callbacks;

    void handleEvent(int, uint32_t);
};