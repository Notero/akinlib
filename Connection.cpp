#include "Connection.hpp"

#include <sys/socket.h>
#include <cerrno>
#include <cstdio>
#include <cctype>
#include <sstream>
#include <cassert>

namespace {
    std::string_view trim(std::string_view view){
        bool tail = false, head = false;
        while(true){
            if (view.length() == 0) return view;
            if(std::isspace(static_cast<unsigned char>(view.front()))){view.remove_prefix(1);}else head = true;
            if (view.length() == 0) return view;
            if(std::isspace(static_cast<unsigned char>(view.back()))){view.remove_suffix(1);}else tail = true;
            if(head && tail) return view;
        }
    }
}

// Returns false if the connection failed.
bool Connection::sendMsg(std::string_view msg) const 
{
    const char* p = msg.data();
    size_t left = msg.size();
    while (left > 0) {
        ssize_t n = send(socketFd, p, left, 0);
        if (n == -1) {
            if (errno == EINTR) continue;  // interrupted by a signal: just retry
            perror("send");
            return false;
        }
        p += n;      // skip past what was sent
        left -= n;   // and send only the rest next time
    }
    return true;
}

// Receives one message and stores it trimmed in `out`.
// Returns false if the client disconnected or the connection failed.
// Note: `out` can be empty if the user just pressed Enter.
    
bool Connection::recvMsg(std::string& out){
    char buf[512];
    ;
    std::string message;

    if(std::getline(std::stringstream(overflowBuffer),message,'\n')){
        out.assign(trim(std::string_view(message.data(), message.length())));
        overflowBuffer.erase(0, message.length() + 1);
        return true;
    }
    

    while (true) {
        bool found = false;
        ssize_t len = recv(socketFd, buf, sizeof(buf), 0);
        if (len == 0) return false;          // client left

        if(len == -1 && errno == EINTR){continue;} else if(len == -1){
            perror("recv");
            return false;
        }

        for (ssize_t i = 0; i < len; i++)
        {
            overflowBuffer.push_back(buf[i]);
            if(buf[i] == '\n'){
                found = true;
            }
        }
        if (found)
        {
            break;
        }
    }

    if(std::getline(std::stringstream(overflowBuffer),message,'\n')){
        out.assign(trim(std::string_view(message.data(), message.length())));
        overflowBuffer.erase(0, message.length() + 1);
        return true;
    }else{
        assert(false && "stream should contain '\n' but doesn't");
        return false;
    }
}