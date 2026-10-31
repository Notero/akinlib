#pragma once

#include <cstddef>
#include <vector>
#include <memory>

class OOPEntity
{
protected:
    size_t id = 0;
public:
    virtual ~OOPEntity() = 0;
    virtual void update() = 0;
};

OOPEntity::~OOPEntity() {}

class OOPArcher : public OOPEntity
{
public:
    virtual ~OOPArcher() override {}
    void update() override {id = 1;}
};

class OOPKnight : public OOPEntity
{
public:
    virtual ~OOPKnight() override {}
    void update() override {id = 2;}
};

class OOPOrc : public OOPEntity
{
public:
    virtual ~OOPOrc() override {}
    void update() override {id = 3;}
};

class OOPOrcArcher : public OOPEntity
{
public:
    virtual ~OOPOrcArcher() override {}
    void update() override {id = 4;}
};



