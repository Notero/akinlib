#pragma once

#include <unistd.h>
#include <string>
#include <string_view>

class Connection{
private:
    int socketFd;
    std::string overflowBuffer;
public:
    explicit Connection(int fd) : socketFd(fd) {}
    ~Connection(){if(socketFd != -1) close(socketFd);}
    //copy
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    //move
    Connection(Connection&& other): socketFd(-1){
        socketFd = other.get_fd();
        other.socketFd = -1;
    }
    //move as in Connection a(5); Connection b(6); b = std::move(a)
    Connection& operator=(Connection&& other){
        if (this == &other) return *this;   // same object? do nothing
        if (socketFd != -1) close(socketFd); // drop what I own
        socketFd = other.socketFd;           // take theirs
        other.socketFd = -1;                 // they own nothing now
        return *this;
    }

    int get_fd() const {return socketFd;}
    bool sendMsg(std::string_view msg) const;
    bool recvMsg(std::string& out);
    bool sendUDP(const struct sockaddr *dest_addr, socklen_t dest_len,std::string_view msg) const;
    bool recvUDP(struct sockaddr *dest_addr, socklen_t& dest_len, std::string& out);
};