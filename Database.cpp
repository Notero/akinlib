#include "Database.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include <mutex>

Database::~Database(){if(db != nullptr) sqlite3_close(db);}

bool Database::exec(const char* sql){
    char *a = nullptr;
    if (SQLITE_OK != sqlite3_exec(db,sql, nullptr, nullptr, &a)){
        if (a) std::cout << a << std::endl;
        sqlite3_free(a);
        return false;
    }
    return true;
}

int Database::openDB(const char *f){
    std::lock_guard l(mtx);
    int x = sqlite3_open_v2(f, &db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,nullptr);
    if (x != SQLITE_OK) {
        std::cout << sqlite3_errmsg(db) << std::endl;
        return x;
    }
    if (!exec("PRAGMA foreign_keys = ON;")) return -1;
    if (!exec("CREATE TABLE IF NOT EXISTS Users(id INTEGER PRIMARY KEY,display_name TEXT NOT NULL, username TEXT UNIQUE NOT NULL, password_hash TEXT NOT NULL, creation_time TEXT DEFAULT CURRENT_TIMESTAMP, last_login TEXT DEFAULT CURRENT_TIMESTAMP);")) return -1;
    if (!exec("CREATE TABLE IF NOT EXISTS ChatLog(id INTEGER PRIMARY KEY,message TEXT NOT NULL,userID INTEGER NOT NULL REFERENCES Users(id), send_time TEXT DEFAULT CURRENT_TIMESTAMP);")) return -1;
    return x;
}

sqlite3_stmt* Database::prep(const char* sql){
    sqlite3_stmt* stmt = nullptr;
    if (SQLITE_OK != sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,nullptr)){
        std::cout << "failed to prep statement: " << sqlite3_errmsg(db) << std::endl;
    }
    return stmt;
}

bool Database::bind_text(sqlite3_stmt* stmt, int index, const std::string& bindee){
    if (SQLITE_OK != sqlite3_bind_text(stmt,index,bindee.data(),bindee.size(),SQLITE_TRANSIENT)){
        std::cout << "failed to bind to statement" << std::endl;
        return false;
    }
    return true;
}    

bool Database::usernameExists(const std::string& u){
    std::lock_guard l(mtx);
    sqlite3_stmt* stmt = prep("SELECT id FROM Users WHERE username = ?;");
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> guard(stmt, &sqlite3_finalize);
    if (stmt == nullptr) return false;
    if(!bind_text(stmt,1,u)) return false;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) return true;
    return false;
}

bool Database::registerUser(const std::string& displayName, const std::string& username, const std::string& passwordHash){
    std::lock_guard l(mtx);
    sqlite3_stmt* stmt = prep("INSERT INTO Users (display_name, username, password_hash) VALUES (?, ?, ?);");
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> guard(stmt, &sqlite3_finalize);
    if (nullptr == stmt){return false;}
    if(!bind_text(stmt,1,displayName)) return false;
    if(!bind_text(stmt,2,username)) return false;
    if(!bind_text(stmt,3,passwordHash)) return false;
    int rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) return false;
    return true;
}

std::optional<Payload> Database::loginUser(const std::string& u){
    std::lock_guard l(mtx);
    Payload pl;
    sqlite3_stmt* stmt = prep("SELECT id, display_name, password_hash FROM Users WHERE username = ?;");
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> guard(stmt, &sqlite3_finalize);
    if (stmt == nullptr) return std::nullopt;
    if(!bind_text(stmt,1,u)) return std::nullopt;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW){
        pl.id = sqlite3_column_int(stmt, 0);
        pl.dn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pl.ph = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        return pl;
    }
    return std::nullopt;
}

