#include "sqlite3.h"
#include <string>
#include <vector>
#include <iostream>

class db_class
{
private:
// variables
    const std::string database_filename = "props.db";
    std::vector<std::string> gas_names;
    sqlite3 *db;
    std::string querry = "select * from *";
    bool Is_database_open = false;
    unsigned int total_gas_in_db = 0;
//class members
    bool open_db();

public:
// variables
    

//class members
    db_class(const char* custom_filename);
    unsigned int print_all_gas();

};

//db_class:: db_class(std::string custom_filename)
db_class:: db_class(const char* custom_filename)
{
    gas_names.reserve(10);
    std::string custom_filename_str = custom_filename;
    if(database_filename != custom_filename_str)
    {
        // break the const promise only this time
        std::string& my_ref = const_cast<std::string&>(database_filename);
        my_ref.erase();
        my_ref = custom_filename;
    }

    // open the database in the constructor
    Is_database_open = open_db();
}

bool db_class:: open_db()
{
    int rc;
    rc = sqlite3_open(database_filename.c_str(), &db);
    if(rc) 
       return false;
    else 
       return true;
}

unsigned int db_class:: print_all_gas()
{
    // check whether db is open
    if (!Is_database_open)
        return 2;
    
    // sql querry
    // select gas_name from base_gas_prop where id<=(SELECT MAX(id) FROM base_gas_prop);
    querry.erase();
    // get total number of gases
    querry = "SELECT MAX(id) FROM base_gas_prop";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    if(sqlite3_step(stmt) == 100)
        total_gas_in_db = (unsigned int)sqlite3_column_int(stmt, 0);
    else 
        return 1;

    querry.erase();
    querry = "select gas_name from base_gas_prop where id<=(SELECT MAX(id) FROM base_gas_prop)";
    sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    std::string gas_name;
    for(unsigned int i=0; i<total_gas_in_db; i++)
    {
        if (sqlite3_step(stmt) == 100)
        {
            gas_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            gas_names.push_back(gas_name);
        }
        else
        {
            return 1;
            break;
        }
        gas_name.erase();
    }

    sqlite3_finalize(stmt);

    for(auto& i: gas_names)
        std::cout<<"\n"<<i;
    return 0;
}