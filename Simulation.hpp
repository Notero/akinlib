#pragma once

#include "SparseSet.hpp"
#include "UIDGenerator.hpp"
#include "Structs.hpp"
#include <chrono>
#include <mutex>
#include <atomic>

class Simulation
{
private:
    UIDGenerator entitygen;
    SparseSet<std::vector<size_t>> owned{500000};
    SparseSet<Velocity> velComp{500000};
    SparseSet<Position> posComp{500000};
    std::chrono::duration<double> accumulator{0.0};
    std::chrono::duration<double> fixedDt{1.0 / 60.0};
    std::mutex mtx;

public:
    //returns entity id so client knows which entities it owns.
    size_t spawn(size_t clientID){
        std::lock_guard lock(mtx);
        size_t id = entitygen.getUID();
        owned.vectorInsert(std::pair(clientID, std::vector<size_t>{id}));
        velComp.insert(std::pair(id,Velocity{0.0,0.0}));
        posComp.insert(std::pair(id,Position{UType::Position,0.0,0.0}));
        return id;
    }
    void inputSpeed(const size_t& entityID, const Velocity& inputVel){
        std::lock_guard lock(mtx);
        Velocity& entityVel = velComp.getComponent(entityID);
        entityVel += inputVel;
    }
    //tick function to update when input updates the velocity component
    void tick(){
        std::lock_guard lock(mtx);
        for(auto& c: velComp){
                Position& pos = posComp.getComponent(c.first);
                Velocity& vel = c.second;
                pos.x += vel.x * fixedDt.count();
                pos.y += vel.y * fixedDt.count();
                if(vel.x > 0 && vel.y > 0){
                    vel -= 1;
                }
        }
    }
    //fixed rate accumulator loop ~60frames per second
    void accumulatorLoop(std::atomic<bool>& shutdownReq){
        auto prev = std::chrono::steady_clock::now();
        while(!shutdownReq){
            auto curr = std::chrono::steady_clock::now();
            std::chrono::duration<double> frameTime = curr - prev;
            prev = curr;
            accumulator += frameTime;
            while (accumulator >= fixedDt)
            {
                tick();
                accumulator -= fixedDt;
            }
        }
    }

};
