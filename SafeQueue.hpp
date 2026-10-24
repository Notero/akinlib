#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>
#include <atomic>

struct Job
{
    /* data */
};

template<typename I>
class SafeQueue
{
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<I> queue;

public:
    std::atomic_bool stopAll = false;

    void ntf_all(){
        cv.notify_all();
    }
    //push
    void safe_add(I item){
        std::lock_guard lock(mtx);
        queue.push(std::move(item));
        cv.notify_one();
    }
    //pop
    std::optional<I> next_job(){
        std::unique_lock lock(mtx);
        cv.wait(lock,[this](){return (!queue.empty() || stopAll.load() == true);});
        if(queue.empty()) return std::nullopt;
        std::optional<I> fr = std::optional<I>(queue.front());
        queue.pop();
        return fr;
    }
    std::optional<I> safe_peek(){
        std::lock_guard lock(mtx);
        if (!queue.empty()) return std::optional<I>(queue.front());
        return std::nullopt;
    }
    bool empty() {
        std::lock_guard lock(mtx);
        return queue.empty();
    }
};
