#pragma once
#include <cmath>

enum class UType : uint8_t {Connect, Disconnect, Input, Position, Nugde, React, Success, Spawn, Snapshot};
enum class RType: uint8_t {Success,Error,Timeout};

struct Position{
    UType type = UType::Position;
    double x,y;    
};

struct Velocity{
    double x,y;
    Velocity& operator-=(int a){
        x -= a;
        y -= a;
        if(x < 0){x = 0;}
        if(y < 0){y = 0;}
        return *this;
    }
    Velocity& operator+=(const Velocity& other){
        x += other.x;
        y += other.y;
        return *this;
    }
};

struct Input{
    UType type = UType::Input;
    Velocity v;
    size_t cID;
    size_t eID;
};

struct Ping{
    UType type;
    size_t id;
};