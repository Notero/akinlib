#include "Pub.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <functional>

int main()
{
    std::atomic<int> work;
    std::condition_variable convar;

    size_t totalWork = 100;

    work.store(totalWork);
    std::function<void()> jobS = [&work, &convar](){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        work--;
        if (work.load() == 0){convar.notify_one();}
    };
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < totalWork; i++)
    {
        jobS();
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << totalWork << " jobs took " << elapsed_t.count() << " ms [serial_baseline]\n";

    std::cout << "Pool results 10 threads" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        Pub newPub(i + 2);
        work.store(totalWork);
        auto start_time = std::chrono::steady_clock::now();
        for (size_t i = 0; i < totalWork; i++)
        {
            std::function<void()> job = [&work, &convar](){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                work--;
                if (work.load() == 0){convar.notify_one();}
            };
            newPub.addJob(std::move(job));
        }
        std::mutex a;
        std::unique_lock b(a);

        convar.wait(b,[&work](){return work == 0;});
        auto end_time = std::chrono::steady_clock::now();

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << i << ". " << totalWork << " jobs took " << elapsed.count() << " ms pooled" << std::endl;
    }
    
    

    return 0;
}
