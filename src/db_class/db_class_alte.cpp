#include "sqlite3.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>

struct CP_Const
{
    float A, B, C, D;
};

class db_access
{
    // if member func return 2 --> db is not open / constructor prob
    // if return 1 --> its the sqlite command fail
private:
// Private variables
    const std::string database_filename = "props.db";
    
    sqlite3 *db;

    std::string gas_choices;

    std::string querry;
    unsigned int total_gas_in_db = 0;

// Private class members


protected:
    std::set<unsigned int> gas_choice_id;
    std::map<unsigned int, std::string> all_gas_names_map;
    bool is_set_created = false;
    bool Is_database_open = false;

public:
// Public variables

// Public class members
    db_access(const char* custom_filename);
    unsigned int get_all_gas_names();
    unsigned int choose_gas_from_user();
    ~db_access();
};


db_access:: db_access(const char* custom_filename)
{
    std::string custom_filename_str = custom_filename;
    if(database_filename != custom_filename_str)
    {
        // break the const promise only this time
        std::string& my_ref = const_cast<std::string&>(database_filename);
        my_ref.erase();
        my_ref = custom_filename;
    }

    // open the database in the constructor

    int rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
    }
    else{
       Is_database_open = true;
    std::cout<<"\n db is opened \n";
    } 
}

db_access:: ~db_access()
{
    std::cout<<"\n\nDestructor has been called\n\n";
    if(Is_database_open)
        sqlite3_close(db);
}

unsigned int db_access:: get_all_gas_names()
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
    querry = "select id, gas_name from base_gas_prop where id<=(SELECT MAX(id) FROM base_gas_prop)";
    sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    for(unsigned int i=0; i<total_gas_in_db; i++)
    {
        // here reinterpret cast is used because column_text func returns const unsigned char*
        if (sqlite3_step(stmt) == 100){
            all_gas_names_map.insert(std::make_pair((unsigned int)sqlite3_column_int(stmt, 0), 
                                                    reinterpret_cast <const char*>(sqlite3_column_text(stmt, 1))));
        }
        else
            return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

unsigned int db_access::choose_gas_from_user()
{
    if(is_set_created){
        std::cout<<"\n\n Gas already been selected \n\n";
        return 0;
    }
    // this part interacts with the user
    std::cout<<"Choose gas : (enter nuber comma or space seperated)\n";

    for(const auto& i : all_gas_names_map)
        std::cout<<"\n"<<i.first<<". "<<i.second;
    
    std::cout<<"\n\nYour choices : ";
    std::getline(std::cin, gas_choices);
    
    // clean the input from user
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

    //DEBUG
    // for(const auto& i : gas_choice_id)
    //     std::cout<<"\t"<<i;

    // once set created set flag true
    is_set_created = true;
    
    // close the db if user done with selecting gas
    sqlite3_close(db);
    Is_database_open = false;
    return 0;
}

// -------------------------------------------------------------

class bip_matrix : private db_access
{
private:
    unsigned int prepare_bip();
    std::unique_ptr<std::vector<std::vector<float>>> bip_pointer;
    std::string querry;
    sqlite3 *db;

public:
    bip_matrix(const char* filename) : db_access(filename){}
    std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();

};

unsigned int bip_matrix::prepare_bip()
{
    if (!Is_database_open)
        return 2;

    // if(!is_set_created)
    //     return 3;

    // querry
    //select gas_name,Argon,CO2,CO,H2O,Methane from bip where gas_name 
    //in ('Argon','CO2','CO','H2O','Methane') order by id ASC

    querry.erase();
    // create querry
    std::string front, second;
    for(const auto& i : gas_choice_id){
        front = front + all_gas_names_map[i] + ',';
        second = second + "'" + all_gas_names_map[i] + "',";

    }
    unsigned int le1 = front.length();
    unsigned int le2 = second.length();
    front.erase(le1-1,le1);
    second.erase(le2-1,le2);

    querry = "select " + front + " from bip where gas_name in (" + second + ") order by id ASC";

    // for DEBUGGING    
    std::cout<<"\n\n BIP querry : \n"<<querry<<"\n";

    //  make ready to querry db
    sqlite3_stmt* stmt;
    std::size_t no_of_choices = gas_choice_id.size();
    std::vector<std::vector<float>> bip_arr(no_of_choices , std::vector<float>(no_of_choices));

    if(sqlite3_prepare_v2(db, querry.c_str(), -1, &stmt, NULL) == SQLITE_OK)
        for(unsigned int i=0; i<no_of_choices; i++){
            if(sqlite3_step(stmt) == 100)
                for(unsigned int j=0; j<no_of_choices; j++)
                    bip_arr[i][j] = (float)sqlite3_column_double(stmt, j);
            else
                return 1;
        }
    else
        return 1;

    // make a unique ptr for the data
    bip_pointer = std::make_unique<std::vector<std::vector<float>>>(bip_arr);

    // DEBUGGING print the bip data
    // for(const auto& i : bip_arr){
    //     for(const auto& j : i)
    //         std::cout<<j<<"\t";
    //     std::cout<<"\n";}
   
    sqlite3_close(db);
    Is_database_open = false;
    return 0;
}

std::unique_ptr<std::vector<std::vector<float>>> bip_matrix::get_bip_pointer()
{
    unsigned int res = prepare_bip();
    std::cout<<"\n\nres : "<<res<<"\n\n";
    if(res==0)
        return std::move(bip_pointer);
    else
        return nullptr;
}