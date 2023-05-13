#include "sqlite3.h"
#include <string>
#include <vector>
#include <set>
#include <array>
#include<iostream>

class db_class
{
private:
// variables
    const std::string database_filename = "props.db";
    
    sqlite3 *db;

    std::vector<std::string> gas_names;
    std::string gas_choices;


    std::string querry = "select * from *";
    bool Is_database_open = false;
    unsigned int total_gas_in_db = 0;

    std::set<unsigned int> gas_choice_id;
//class members
    bool open_db();
    ~db_class();
    void gas_choice_str2Vec();
    void choosen_gas_querry();
    

public:
// variables

//class members
    db_class(const char* custom_filename);
    unsigned int get_all_gas_names();
    unsigned int prepare_bip();

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

db_class:: ~db_class()
{
    std::cout<<"\ndestructor been called\n";
    sqlite3_close(db);
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

unsigned int db_class:: get_all_gas_names()
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

    //querry returns all the gas name in base_prop table
    querry = "select gas_name from base_gas_prop where id<=(SELECT MAX(id) FROM base_gas_prop)";
    sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    for(unsigned int i=0; i<total_gas_in_db; i++)
    {
        // here reinterpret cast is used because column_text func returns const unsigned char*
        if (sqlite3_step(stmt) == 100)
            gas_names.push_back(reinterpret_cast <const char*>(sqlite3_column_text(stmt, 0)));
        else
            return 1;
    }

// this part interacts with the user
    std::cout<<"Choose gas : (enter nuber comma seperated / to choose all just 1-last_gas_no )\n";
    unsigned int j=1;
    for(auto i: gas_names)
        std::cout<<"\n"<<j++<<". "<<i;
    
    std::cout<<"\n\nYour choices : ";
    std::getline(std::cin, gas_choices);

    sqlite3_finalize(stmt);
    return 0;
}

void db_class::gas_choice_str2Vec()
{
    // clean the string directly comparing with ACSII values
    for(auto& i : gas_choices){
        if(int(i)<44)
            i=',';
        else if(int(i)>57)
            i=',';
        else if(int(i)>44 && int(i)<48)
            i=',';
        else
            continue;
    }

// seperate choices to set since set dont allow duplictae and ordered in nature
    std::string items;
    for(auto& i : gas_choices){
        if(int(i)>47){
            items = items + i;
            continue;
        }
        if(items.length()>0){
            int j = stoi(items);
            if(j<=total_gas_in_db)
                gas_choice_id.insert(j);
        }
        items.erase();
    }

    int j = stoi(items);
    if(j<=total_gas_in_db)
        gas_choice_id.insert(j);

    // for(const auto& i : gas_choice_id)
    //     std::cout<<"\t"<<i;
}

void db_class::choosen_gas_querry()
{
    gas_choice_str2Vec();

    // querry
    // select gas_name, Hydrogen, Methane, Ammonia from bip where 
    // gas_name = 'Methane' or gas_name = 'Hydrogen' or gas_name = 'Ammonia';

    std::string new_q;
    std::string new_q1;
    for(const auto& i : gas_choice_id){
        new_q = new_q + gas_names[i-1] + ',';
        new_q1 = new_q1 + "gas_name = '" + gas_names[i-1] + "' or ";
    }
    new_q[new_q.length() - 1] = 0;
    
    unsigned int l = new_q1.length();
    new_q1[l - 1] = 0;
    new_q1[l - 2] = 0;
    new_q1[l - 3] = 0;
    querry.erase();
    querry = "select " + new_q + " from bip where " + new_q1;
    //std::cout<<"\n\n"<<querry;
}

unsigned int db_class::prepare_bip()
{
    // TODO 2d array system using array again
    choosen_gas_querry();
    sqlite3_stmt* stmt;
    std::cout<<"\n"<<querry<<"\n";
    sqlite3_prepare_v3(db, querry.c_str(), 200, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    // for(unsigned int i=0; i<total_gas_in_db; i++)
    //     for(unsigned int j=0; j<total_gas_in_db; j++)
            // if(sqlite3_step(stmt) == 100)
            std::cout<<"\n"<<sqlite3_step(stmt)<<"\n";
            std::cout<<sqlite3_column_type(stmt, 0);
    
    return 0;
}