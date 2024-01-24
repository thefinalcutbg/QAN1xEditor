#pragma once

#include <string>

struct sqlite3;
struct sqlite3_stmt;

class Db
{
    sqlite3* db_connection{ nullptr };
    bool m_connectionOwned{ false };

    int total_bindings{ 0 };
    int successful_bindings{ 0 };

    sqlite3_stmt* stmt{ nullptr };

    static inline bool s_showError{ false };


    void finalizeStatement();

    static inline std::string location;// = "default.an2";

public:
    static std::string getDbPath() { return location; }
    static int version();
    static void setVersion(int version);
    //open new connection and execute query on the go
    static bool createIfNotExist();
    static void showErrorDialog(bool show) {s_showError = show;}

    //If connection exists, db finalizes statement in destructor, but does not break connection
    Db(Db* existingConnection = nullptr);
    //Create db connection to external location:
    Db(const std::string& path);
     
    //returns true if there are more rows to get from database
    bool hasRows(); 
    int asInt(int column);
    long long asRowId(int column);
    long long asLongLong(int column);
    bool asBool(int column);
    double asDouble(int column);
    const void* asBlob(int column);
    std::string asString(int column);
    int getColumnSize(int column);
    void newStatement(const std::string& query);
    bool execute(const std::string& query);

    long long lastInsertedRowID();
    std::string getPreparedStatement();
    void closeConnection();


    //bindings with prepared statement:
    void bind(int index, const std::string& value);
    void bind(int index, int value);
    void bind(int index, double value);
    void bind(int index, long long value);
    void bind(int index, const void* ptr, int size);
    void bindNull(int index);
    //executes already prepared statement with bindings
    bool execute();

    ~Db();

};

