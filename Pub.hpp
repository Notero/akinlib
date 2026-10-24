#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include "SafeQueue.hpp"

class Pub
{
private:
    std::vector<std::thread> pool;
    SafeQueue<std::function<void()>> sq;

    std::function<void()> worker = [this](){
        {
            while (true)
            {
                auto f = sq.next_job();
                //execute the job
                if(f) (*f)();
                if (!f) break;
            }
        }
    };
public:

    Pub(size_t count){
        for (size_t i = 0; i < count; i++){
            std::thread t(worker);
            pool.push_back(std::move(t));
        }
    }

    ~Pub(){
        sq.stopAll.store(true);
        sq.ntf_all();

        for(auto& a : pool){
            a.join();
        }
    };

    void addJob(std::function<void()> j){sq.safe_add(std::move(j));}
};
