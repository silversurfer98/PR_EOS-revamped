#include "sqlite3.h"
#include <string>
#include <vector>
#include <set>
#include<memory>
#include<iostream>

struct CP_Const
{
    float A, B, C, D;
};

class db_class
{
private:
// Private variables
    const std::string database_filename = "props.db";
    
    sqlite3 *db;

    std::vector<std::string> gas_names;
    std::string gas_choices;


    std::string querry = "select * from *";
    bool Is_database_open = false;
    unsigned int total_gas_in_db = 0;

    std::set<unsigned int> gas_choice_id;

    std::unique_ptr<std::vector<std::vector<float>>> bip_pointer;
    std::unique_ptr<std::vector<CP_Const>> cp_const_vals;


    unsigned int process_count = 0;
// Private class members
    ~db_class();
    void gas_choice_str2Vec();
    unsigned int choosen_gas_bip_querry();
    unsigned int prepare_bip();
    void choose_gas_from_user();
    

public:
// Public variables

// Public class members
    db_class(const char* custom_filename);
    unsigned int get_all_gas_names();
    std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();
    unsigned int cp_const_data_aquisition();


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

    int rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        Is_database_open = false;
    }
    else 
       Is_database_open = true;
}

db_class:: ~db_class()
{
    sqlite3_close(db);
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

    sqlite3_finalize(stmt);
    return 0;
}

void db_class::choose_gas_from_user()
{
    // this part interacts with the user
    std::cout<<"Choose gas : (enter nuber comma seperated / to choose all just 1-last_gas_no )\n";
    unsigned int j=1;
    for(auto i: gas_names)
        std::cout<<"\n"<<j++<<". "<<i;
    
    std::cout<<"\n\nYour choices : ";
    std::getline(std::cin, gas_choices);
    process_count++;
}

void db_class::gas_choice_str2Vec()
{
    if(process_count==0)
        choose_gas_from_user();
    
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
}

unsigned int db_class::choosen_gas_bip_querry()
{
    if(gas_choice_id.size()==0)
        gas_choice_str2Vec();

    // querry
    // select gas_name, Hydrogen, Methane, Ammonia from bip where 
    // gas_name = 'Methane' or gas_name = 'Hydrogen' or gas_name = 'Ammonia';

    std::string new_q;
    std::string new_q1;
    /*** 
     * I didnt know about iterators so I cretaed a new set in reverse order
    //std::set<unsigned int, std::greater<unsigned int>> gas_choice_id_rev(gas_choice_id.begin(), gas_choice_id.end()); 
    //for(const auto& i : gas_choice_id_rev)
    ***/
    for(auto i = gas_choice_id.rbegin(); i != gas_choice_id.rend(); i++)
        new_q = new_q + gas_names[*i-1] + ',';

    for(const auto& i : gas_choice_id)
            new_q1 = new_q1 + "gas_name = '" + gas_names[i-1] + "' or ";
    
    unsigned int l1 = new_q1.length();
    unsigned int l2 = new_q.length();
    new_q1.erase(l1-4,l1);
    new_q.erase(l2-1,l2);

    // std::cout<<"\n"<<new_q<<"\nlength : "<<new_q.length()<<"\n";
    // std::cout<<"\n"<<new_q1<<"\nlength : "<<new_q1.length()<<"\n";

    querry.erase();
    querry = "select " + new_q + " from bip where (" + new_q1 + ')';

// for DEBUGGING    
    // std::cout<<"\n\n"<<querry;
    return 0;
}

unsigned int db_class::prepare_bip()
{
    if (!Is_database_open)
        return 2;

    unsigned int res = choosen_gas_bip_querry();
    if(res!=0)
        return 2;

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

    bip_pointer = std::make_unique<std::vector<std::vector<float>>>(bip_arr);

// print the bip data
    // for(const auto& i : bip_arr){
    //     for(const auto& j : i)
    //         std::cout<<j<<"\t";
    //     std::cout<<"\n";}
   
    return 0;
}

std::unique_ptr<std::vector<std::vector<float>>> db_class::get_bip_pointer()
{
    unsigned int res = prepare_bip();
    if(res==0)
        return std::move(bip_pointer);
    else
        return NULL;
}

unsigned int db_class::cp_const_data_aquisition()
{
    if(gas_choice_id.size() == 0)
        gas_choice_str2Vec();

    std::vector<CP_Const> cp_const_data(5);
    querry.erase();
    querry = "select * from c_ideal where gas_name=?";
    sqlite3_stmt* stmt;

    for(const auto& i : gas_choice_id){
        sqlite3_prepare_v2(db, querry.c_str(), -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, gas_names[i-1].c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == 100){
            CP_Const temp { (float)sqlite3_column_double(stmt, 2),
                            (float)sqlite3_column_double(stmt, 3),
                            (float)sqlite3_column_double(stmt, 4),
                            (float)sqlite3_column_double(stmt, 5)};
            cp_const_data.push_back(temp);
        }
        sqlite3_finalize(stmt);

    }
    for(const auto& i : cp_const_data){
        std::cout<<i.A<<"\t"<<i.B<<"\t"<<i.C<<"\t"<<i.D<<"\n";
    }
}