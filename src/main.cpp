#include "headers/server.h"
#include "headers/commands.h"
#include <csignal>
#include <iostream>
#include <atomic>

std::atomic<bool> g_running(true);

void signalHandler(int sig){
    std::cout << "\nSignal " << sig << " received. Stopping server...\n";
    g_running = false;
}

int main(){
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::string ip = "127.0.0.1";
    uint16_t port = 8080;
    bool useUDP = false;

    Server server(ip, port, useUDP);

    server.setOnClientConnected([](int fd, const sockaddr_in& addr){
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
        std::cout << "Client connected:\n" << ip << ":" << ntohs(addr.sin_port) << "\n(fd=" << fd << ")\n";
    });

    server.setOnDataReceived([&server](int fd, const void* data, size_t size){
        std::string msg((const char*)data, size);

        while(!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
            msg.pop_back();
        
        std::string response;

        if(!msg.empty() && msg[0] == '/'){
            std::string cmd = msg.substr(1);
            CommandResult res = handle(cmd, server.getStats(), response);

            if(res == CommandResult::SHUTDOWN){
                std::cout << "Shutdown command received. Stoping server...\n";
                server.stop();
            }
        } else response = msg + "\n";

        if(!response.empty())
            server.sendTo(fd, response.c_str(), response.size());
    });

    if(!server.start()){
        std::cerr << "Failed to start server\n";
        return 1;
    }

    std::cout << "Server started on " << ip << ":" << port << "\n";

    while(g_running){
        server.run();
    }

    server.stop();
    std::cout << "Server stopped\n";
    return 0;
}