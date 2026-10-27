#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <cstdio>

class UIDGenerator
{
private:
    //ids index
    std::vector<bool> entities;
    //keeps deleted slots which are empty
    std::queue<size_t> _tracker;

public:

    bool alive(size_t id){return entities[id];}
    size_t getUID(){
        if (_tracker.empty())
        {
            entities.push_back(true);
            return entities.size() - 1;
        }
        //if anything is deleted before fill it
        size_t temp = _tracker.front();
        entities[temp] = true;
        _tracker.pop();
        return temp;
    }
    void delUID(size_t id){
        entities[id] = false;
        _tracker.push(id);
    }
};
