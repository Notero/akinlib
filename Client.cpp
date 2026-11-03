#include "Client.hpp"
#include <stdexcept>
#include <cstring>

namespace{
    //signal for option+c or shutdown
    void onSigint(int) {
        shutdownReq = true;
    }
}

void Client::connect(){
    size_t max = 15;
    size_t curr = 0;
    Ping z{};
    std::string conRes;
    sockaddr_in empty{};
    socklen_t esize = sizeof(empty);
    sockaddr* ssp = reinterpret_cast<sockaddr*>(&empty);

    while (true)
    {
        Ping c{UType::Connect};
        std::string_view view(reinterpret_cast<const char*>(&c), sizeof(c));
        client->sendUDP(sp,len,view);
        RType r = client->recvUDP(ssp,esize,conRes);
        if (r == RType::Error){throw std::runtime_error("recv on Client::connect() error.");}
        if (curr == max){throw std::runtime_error("recv on Client::connect() timeout.");}
        if (r == RType::Success) {break;}
        curr++;
    }
    std::memcpy(&z,conRes.data(),sizeof(z));

    ID = z.id;
}

void Client::disconnect(){
    Ping d{UType::Disconnect, ID};
    std::string_view dview(reinterpret_cast<const char*>(&d), sizeof(d));
    client->sendUDP(sp,len,dview);
}

void Client::spawnReq(){
    Ping p{UType::Spawn, ID};
    std::string_view v(reinterpret_cast<const char*>(&p),sizeof(p));
    client->sendUDP(sp,len,v);
}

void Client::speedReq(double x, double y,size_t entityID){
    Input i{UType::Input, Velocity{x,y}, ID, entityID};
    std::string_view v(reinterpret_cast<const char*>(&i),sizeof(i));
    client->sendUDP(sp, len, v);
}

std::vector<size_t> Client::getOwned(){return eID;}

void Client::GUI(){
    while (!shutdownReq)
    {
        std::cout << "Hello |" << std::endl;
        std::cout << "-------" << std::endl;
        std::cout << "1. Spawn Ball, 2. Add Speed to A ball, 3. Disconnect" << std::endl;
        int a;
        std::cin >> a;
        switch (a)
        {
        case 1:
            {
                spawnReq();
            }
            break;
        case 2:
            {
                std::cout << "ID list for owned balls" << std::endl;
                {
                    std::lock_guard lock(mtx);
                    for(auto& entity: eID){
                        std::cout << entity << std::endl;
                    }
                }
                double x,y;
                size_t entityID;
                std::cout << "-----------------------------------" << std::endl;
                std::cout << "Enter entity ID from the list above" << std::endl;
                std::cin >> entityID;
                std::cout << "Enter speed towards x" << std::endl;
                std::cin >> x;
                std::cout << "Enter speed towards y" << std::endl;
                std::cin >> y;
                speedReq(x,y,entityID);
            }
            break;
        case 3:
            {
                shutdownReq.store(true);
            }
            break;        
        default:
            std::cout << "Wrong Input" << std::endl;
            break;
        }
    }
}

void Client::receiver(){
    sigset_t a;
    sigemptyset(&a);
    sigaddset(&a, SIGINT);
    int pth = pthread_sigmask(SIG_BLOCK,&a,nullptr);
    if(0 != pth){
        std::cerr << std::strerror(pth) << std::endl;
        return;
    }

    sockaddr_in sip{};
    socklen_t slen = sizeof(sip);
    sockaddr* sp = reinterpret_cast<sockaddr*>(&sip);
    
    unsigned int tickCount = 0;
    while (!shutdownReq)
    {
        std::string response;
        client->recvUDP(sp,slen,response);

        UType type;

        std::memcpy(&type,response.data(),sizeof(type));
        
        switch (type)
        {
        case UType::Nugde:
            {
                Ping react{UType::React,ID};
                std::string_view view(reinterpret_cast<const char*>(&react), sizeof(react));
                client->sendUDP(sp, slen, view);
            }
            break;
        case UType::Snapshot:
            {
                // Later, on the receiving side:
                size_t count;
                std::memcpy(&count, response.data() + sizeof(UType), sizeof(count));
                std::vector<std::pair<size_t, Position>> rv(count);
                std::memcpy(rv.data(), response.data() + sizeof(count) + sizeof(UType), count * sizeof(std::pair<size_t, Position>));
                Snapshot s{rv};
                {
                    std::lock_guard lock(mtx);
                    snapshots.push(s);
                }


            }
            break;
        case UType::Spawn:
            {
                Ping spawn{};
                std::memcpy(&spawn,response.data(),sizeof(spawn));
                {
                    std::lock_guard lock(mtx);
                    eID.push_back(spawn.id);
                }
            }
            break;
        case UType::Disconnect:
            {
                Ping disc{};
                std::memcpy(&disc,response.data(),sizeof(disc));
                if (disc.id == ID)
                {
                    return;
                }
            }
            break;
        default:
            break;
        }


    }
}

void Client::startReceiver(){
    std::thread a(&Client::receiver, this);
    receiverThread = std::move(a);
}

Client::Client() : snapshots(10)
{
    //These are configured for my server
    client = std::make_unique<Connection>(socket(AF_INET, SOCK_DGRAM, 0));
    if (client->get_fd() == -1){
        throw std::runtime_error("socket creation failed");
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(3300);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dest = server;
    len = sizeof(dest);
    sp = reinterpret_cast<sockaddr*>(&dest);
    //signal connection SIGINT means Ctrl + c or option + c in macos
    struct sigaction sa{};          // zero it
    sa.sa_handler = onSigint;       // which function to call
    sa.sa_flags = 0;                // 0 = no SA_RESTART, so accept gets interrupted
    if(sigaction(SIGINT, &sa, nullptr) == -1){
        perror("sigaction");
    }
    setsockopt(client->get_fd(),SOL_SOCKET,SO_RCVTIMEO,&on,sizeof(on));
}

Client::~Client(){
    
    disconnect();
    if(receiverThread.joinable()){
        receiverThread.join();
    }
}