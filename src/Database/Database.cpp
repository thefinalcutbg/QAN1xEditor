﻿#include "Database.h"

#include <sqLite3/sqlite3.h>
#include <qdebug.h>
#include <filesystem>

constexpr const char* database_error_msg = "Couldnt create database";

Db::Db(Db* existingConnection)
    :
    m_connectionOwned{ existingConnection == nullptr },
    stmt{nullptr}
{
    if(m_connectionOwned){
        int i = sqlite3_open(dbLocation.c_str(), &db_connection);
    }
    else
    {
        db_connection = existingConnection->db_connection;
    }
    
    if (db_connection == nullptr) {
         throw;
    }

    execute("PRAGMA foreign_keys = ON");
    
}

Db::Db(const std::string& query, Db* existingConnection) : Db(existingConnection)
{
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}

bool Db::hasRows(){

    if (total_bindings != successful_bindings) return false;

    bool result = sqlite3_step(stmt) == SQLITE_ROW;//|| sqlite3_step(stmt) != ;

    if(!result) finalizeStatement();

    return result;
}

int Db::asInt(int column){ 
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int(stmt, column); 
}

long long Db::asRowId(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int64(stmt, column);
}

long long Db::asLongLong(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int64(stmt, column);
}

bool Db::asBool(int column)
{   
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return false;
    return static_cast<bool>(asInt(column));
}

double Db::asDouble(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_double(stmt, column);
}

const void* Db::asBlob(int column)
{
    return sqlite3_column_blob(stmt, column);
}

std::string Db::asString(int column) { 
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return "";
    return reinterpret_cast<const char*>(sqlite3_column_text(stmt, column)); 
}

int Db::getColumnSize(int column)
{
    return sqlite3_column_bytes(stmt, 1);
}


void Db::newStatement(const std::string& query)
{ 
    finalizeStatement();
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}
//#include <qdebug.h>
bool Db::execute(const std::string& query)
{
    char* err;
    finalizeStatement();
    int i = sqlite3_exec(db_connection, query.c_str(), NULL, NULL, &err);
   // qDebug() << query.c_str();
    if (err && s_showError) {
        //SHOW ERROR
    }

    finalizeStatement();

    return i == SQLITE_OK;
        
}

long long Db::lastInsertedRowID()
{
    return sqlite3_last_insert_rowid(db_connection);
}

std::string Db::getPreparedStatement()
{
   char* ptr = sqlite3_expanded_sql(stmt);

   int size = 0;
   
   sqlite3_malloc(size);

   if (!size) return "";

   std::string result(ptr, size);

   sqlite3_free(ptr);

   return result;
}

void Db::closeConnection()
{
    if (!db_connection) return;

    if (stmt) sqlite3_reset(stmt);

    successful_bindings = 0;
    total_bindings = 0;

     sqlite3_close_v2(db_connection);
}


void Db::bind(int index, const std::string& value)
{
    if(stmt == nullptr) return;

    total_bindings++;
    

    successful_bindings +=
        sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
}

void Db::bind(int index, int value)
{
    if (stmt == nullptr) return;

    total_bindings++;

    successful_bindings +=
        sqlite3_bind_int(stmt,index,value) == SQLITE_OK;
}

void Db::bind(int index, double value)
{
    if (stmt == nullptr) return;

    total_bindings++;

    successful_bindings +=
        sqlite3_bind_double(stmt, index, value) == SQLITE_OK;
}

void Db::bind(int index, long long value)
{
    if (stmt == nullptr) return;

    total_bindings++;

    successful_bindings +=
        sqlite3_bind_int64(stmt, index, value) == SQLITE_OK;
}

void Db::bind(int index, const void* ptr, int size)
{
    if (stmt == nullptr) return;

    total_bindings++;


    successful_bindings +=
        sqlite3_bind_blob(stmt, index, ptr, size, SQLITE_STATIC) == SQLITE_OK;
}

void Db::bindNull(int index)
{
    if (stmt == nullptr) return;

    total_bindings++;

    successful_bindings += sqlite3_bind_null(stmt, index) ? 0 : 1;
}

bool Db::execute()
{
    if (stmt == nullptr) return false;

    if (total_bindings != successful_bindings)
    {
        finalizeStatement();
        return false;
    }

    auto result = sqlite3_step(stmt);
    finalizeStatement();

    return result == SQLITE_DONE;
}


void Db::finalizeStatement()
{
    total_bindings = 0;
    successful_bindings = 0;

    if (!stmt) return;

    sqlite3_finalize(stmt);
    stmt = nullptr;
    
}

void Db::setFilePath(const std::string& filePath)
{
    dbLocation = filePath;
}

int Db::version()
{
    Db db("PRAGMA user_version");

    while (db.hasRows()) {
        return db.asLongLong(0);
    }

    return -1;
}

void Db::setVersion(int version)
{
    Db::crudQuery("PRAGMA user_version =" + std::to_string(version));
}

bool Db::crudQuery(const std::string& query)
{
    char* err;
    sqlite3* db;

    int i = sqlite3_open(dbLocation.c_str(), &db);
    if (db == nullptr) {
        return false;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, &err);

    i = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (err && s_showError) {
       // ModalDialogBuilder::showError(database_error_msg);
    }

    return i == SQLITE_OK;
}


Db::~Db()
{
    if (stmt != nullptr) {
        sqlite3_finalize(stmt);
    }

    if (m_connectionOwned && db_connection) {
        sqlite3_close_v2(db_connection);
    }
}

const char* tableSchema = "CREATE TABLE patch(rowid INTEGER PRIMARY KEY, type TEXT, name TEXT, file TEXT, created_by TEXT, artist TEXT, song TEXT, comment TEXT, data BLOB)";

#include <QFileInfo>
#include <QDir>

bool Db::createIfNotExist()
{
    //if (std::filesystem::exists(dbLocation)) return true;
    
    QFileInfo db_path(dbLocation.c_str());
    if (db_path.exists() && db_path.isFile()) return true;

    QDir d = db_path.absoluteDir();
    d.mkpath(d.absolutePath());

    Db db;

    db.execute(tableSchema);

    return true;

  //  rc = sqlite3_exec(db,"VACUUM", NULL, NULL, &err);

}