#pragma once

#include <sys/socket.h>   // socket, bind, listen, accept, setsockopt, shutdown
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY, htons/htonl
#include <arpa/inet.h>    // inet_pton, inet_ntop (IP string <-> binary)
#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>
#include <atomic>
#include <cstdio>
#include <pthread.h>
#include <chrono>
#include <csignal>
#include "Connection.hpp"
#include "Structs.hpp"
#include "ORingBuffer.hpp"

std::atomic<bool> shutdownReq = false;
using TimePoint = std::chrono::steady_clock::time_point;

struct Snapshot{
    std::vector<std::pair<size_t, Position>> snapshot;
    TimePoint timestamp = std::chrono::steady_clock::now();
};

class Client
{
private:
    size_t ID;
    std::vector<size_t> eID;
    std::thread receiverThread;
    std::mutex mtx;
    ORingBuffer<Snapshot> snapshots;
    std::unique_ptr<Connection> client;
    sockaddr_in dest;
    socklen_t len;
    sockaddr* sp;
    
public:
    explicit Client();
    ~Client();


    void GUI();
    void Connect();
    void Disconnect();
    void receiver();
    void startReceiver();
    
};
