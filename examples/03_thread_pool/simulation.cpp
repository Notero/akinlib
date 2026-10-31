#include <iostream>
#include <sys/socket.h>   // socket, bind, listen, accept, setsockopt, shutdown
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY, htons/htonl
#include <arpa/inet.h>    // inet_pton, inet_ntop (IP string <-> binary)
#include <thread>
#include "Connection.hpp"

void bot(Connection con, size_t i){
    std::string nam = "bot";
        //select register
    con.sendMsg("register\n");

    nam.append(std::to_string(i));

    con.sendMsg(std::string_view(nam+ "\n"));
    con.sendMsg(std::string_view(nam+ "\n"));
    con.sendMsg(std::string_view(nam+ "\n"));
    con.sendMsg(std::string_view(nam+ "\n"));

    con.sendMsg("login\n");

    con.sendMsg(std::string_view(nam+ "\n"));
    con.sendMsg(std::string_view(nam+ "\n"));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    con.sendMsg("this message from " + nam + "\n");

    timeval tv{0, 200000}; // 200ms
    setsockopt(con.get_fd(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    char drain[4096];
    while (recv(con.get_fd(), drain, sizeof(drain), 0) > 0) {}
}


int main()
{
    std::vector<std::thread> av;
    size_t xmany = 50;
    for (size_t i = 0; i < xmany; i++)
    {
    int b = socket(AF_INET,SOCK_STREAM,0);
    if (b == -1)
    {
        return 0;
    }

    //bind options
    sockaddr_in s{};
    //AF_INET address family internet ipv4
    //AF_INET6 address family internet ipv6
    s.sin_family = AF_INET;
    //htons: host to network byte order (Mac is little-endian, network is big-endian)
    s.sin_port = htons(3300);
    //htonl: same for 32-bit (long) values
    s.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    const sockaddr* sp = reinterpret_cast<sockaddr*>(&s);

    if (connect(b,sp, sizeof(s)) == -1){
        return 0;
    }

    Connection c(b);
    int on = 1;

    if (setsockopt(c.get_fd(),SOL_SOCKET,SO_NOSIGPIPE,&on,sizeof(on)) == -1){
        perror("setsockopt");
        continue;
    }

    std::thread t(bot,std::move(c),i);
    av.push_back(std::move(t));
    }

    for(auto& a: av){
        a.join();
    }

    return 0;
}


