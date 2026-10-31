#include <memory>
#include <vector>
#include <cstdio>
#include <iostream>
#include <chrono>
#include "OOPEntity.hpp"
#include "SparseSet.hpp"

struct Benchmark
{
    int id;
    void update() {id = 1;}
};

struct Position
{
    int id;
    void update() {id = 1;}
};

struct Velocity
{
    int id;
    void update(){id = 1;}
};


int main()
{
    std::vector<std::unique_ptr<OOPEntity>> entities;
    for (size_t i = 0; i < 250000; i++)
    {
        entities.push_back(std::make_unique<OOPArcher>());
    }
    for (size_t i = 0; i < 250000; i++)
    {
        entities.push_back(std::make_unique<OOPKnight>());
    }
    for (size_t i = 0; i < 250000; i++)
    {
        entities.push_back(std::make_unique<OOPOrcArcher>());
    }
    for (size_t i = 0; i < 250000; i++)
    {
        entities.push_back(std::make_unique<OOPOrc>());
    }
    auto begin = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 60; i++)
    {
        for(auto& a: entities){
            a->update();
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "OOP benchmark => " << elapsed << std::endl;
    const size_t maxId = 1000001;
    SparseSet<Benchmark> set(maxId);
    SparseSet<Position> set2(maxId);
    SparseSet<Velocity> set3(maxId);
    Benchmark bench;
    bench.id = 0;
    Position pos;
    pos.id = 0;
    Velocity vel;
    vel.id = 0;

    for (size_t i = 0; i < 1000000; i++)
    {
        set.insert(std::pair<size_t,Benchmark>(i, bench));  
    }
    for (size_t i = 0; i < 250000; i++)
    {
        set2.insert(std::pair<size_t,Position>(i, pos));
    }
    for (size_t i = 0; i < 500000; i++)
    {
        set3.insert(std::pair<size_t,Velocity>(i, vel));
    }
    
    /* 
    ideally we loop over the sets with a 
    std::vector<unique_ptr<SparseSet<typename(like a component that all structs extend from)>>>
    so it automatically queries each set and if it finds and component it updates
    */
    auto begin_t = std::chrono::steady_clock::now();
    for (size_t i = 0; i < 60; i++)
    {
        for(auto& a: set){
            a.second.update();
        }
        for(auto& a: set2){
            a.second.update();
        }
        for(auto& a: set3){
            a.second.update();
        }
    }
    auto end_t = std::chrono::steady_clock::now();
    auto elapsed_t = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - begin_t);

    std::cout << "ECS benchmark => " << elapsed_t << std::endl;

    

    return 0;
}