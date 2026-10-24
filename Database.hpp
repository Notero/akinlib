#pragma once

#include <sqlite3.h>
#include <string>
#include <optional>
#include <mutex>

struct Payload
{
    int id;
    std::string dn;
    std::string ph;
};

class Database
{
private:
    std::mutex mtx;
    //private database pointer
    sqlite3* db = nullptr;
    //prep bind execute
    sqlite3_stmt* prep(const char* sql);
    bool bind_text(sqlite3_stmt* stmt, int index, const std::string& bindee);
    bool exec(const char* sql);
public:

    //deleted cpy opt and set destructor to RAII
    Database() = default;
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    
    //db connection
    int openDB(const char *f);
    //login & register API
    bool usernameExists(const std::string& u);
    bool registerUser(const std::string& displayName, const std::string& username, const std::string& passwordHash);
    std::optional<Payload> loginUser(const std::string& u);
};