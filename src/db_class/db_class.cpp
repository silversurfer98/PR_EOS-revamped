#include "sqlite3.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>

// data structs
struct CP_Const
{
    float A, B, C, D;
};

//PR EOS properties definition
struct PR_props
{
    float a, b, aa, bb, k, alpha, ac, c, d, e;
};

//gas base properties definition
struct base_props
{
    float tc, pc, w;
};



class db_class
{
private:
// Private variables
    const std::string database_filename = "props.db";
    
    sqlite3 *db;

    std::map<unsigned int, std::string> all_gas_names_map;
    std::string gas_choices;

    std::string querry;
    bool Is_database_open = false;
    unsigned int total_gas_in_db = 0;

    std::set<unsigned int> gas_choice_id;
    bool is_set_created = false;

    std::unique_ptr<std::vector<std::vector<float>>> bip_pointer;

    std::unique_ptr<std::vector<CP_Const>> cp_const_vals;

    std::unique_ptr<std::vector<base_props>> base_gas_props_ptr;

// Private class members
    unsigned int prepare_bip();
    unsigned int cp_const_data_aquisition();
    unsigned int get_base_gas_props();
    

public:
// Public variables
    bool is_mix = true;
    unsigned int size_of_gas_data = 1;

// Public class members
    db_class(const char* custom_filename);
    unsigned int get_all_gas_names();
    unsigned int choose_gas_from_user();
    std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();
    std::unique_ptr<std::vector<CP_Const>> get_cp_const_pointer();
    std::unique_ptr<std::vector<base_props>> get_base_gas_props_ptr();
    virtual ~db_class();

};

//db_class:: db_class(std::string custom_filename)
db_class:: db_class(const char* custom_filename)
{
    std::cout<<"\nDB_class constructor has been called\n";
    std::string custom_filename_str = custom_filename;
    if(database_filename != custom_filename_str)
    {
        // break the const promise only this time
        std::string& my_ref = const_cast<std::string&>(database_filename);
        my_ref.erase();
        my_ref = custom_filename;
    }

    // open the database in the constructor

    // int rc = sqlite3_open(database_filename.c_str(), &db);
    // if (rc != SQLITE_OK) {
    //     std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
    //     sqlite3_close(db);
    //     Is_database_open = false;
    // }
    // else 
    //    Is_database_open = true;
}

db_class:: ~db_class()
{
    std::cout<<"\n\nDB_class destructor has been called\n\n";
    // sqlite3_close(db);
}

unsigned int db_class:: get_all_gas_names()
{
    // check whether db is open
    // if (!Is_database_open)
    //     return 2;
    
    // sql querry
    // select gas_name from base_gas_prop where id<=(SELECT MAX(id) FROM base_gas_prop);
    querry.erase();
    querry = "SELECT MAX(id) FROM base_gas_prop";
    int rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return 1;
    }

    // get total number of gases
    sqlite3_stmt* stmt;
    sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    if(sqlite3_step(stmt) == 100)
        total_gas_in_db = (unsigned int)sqlite3_column_int(stmt, 0);
    else 
        return 2;

    querry.erase();

    //querry returns all the gas name in base_prop table
    // querry = "select id, gas_name from base_gas_prop where id<=(SELECT MAX(id) FROM base_gas_prop)";
    // sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);

    querry = "select id, gas_name from base_gas_prop where id<=?";
    sqlite3_prepare_v3(db, querry.c_str(), 100, SQLITE_PREPARE_PERSISTENT, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, (int)total_gas_in_db);

    for(unsigned int i=0; i<total_gas_in_db; i++)
    {
        // here reinterpret cast is used because column_text func returns const unsigned char*
        if (sqlite3_step(stmt) == 100){
            all_gas_names_map.insert(std::make_pair((unsigned int)sqlite3_column_int(stmt, 0), 
                                                    reinterpret_cast <const char*>(sqlite3_column_text(stmt, 1))));
        }
        else
            return 2;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

unsigned int db_class::choose_gas_from_user()
{
    if(is_set_created){
        std::cout<<"\n\n Gas already been selected \n\n";
        return 1;
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

    // after loop the item contains last element of gas_choice string
    int j = stoi(items);
    if(j<=total_gas_in_db)
        gas_choice_id.insert(j);

    //DEBUG
    // for(const auto& i : gas_choice_id)
    //     std::cout<<"\t"<<i;

    // once set created set flag true
    is_set_created = true;
    size_of_gas_data = gas_choice_id.size();
    if(size_of_gas_data == 1){
        std::cout<<"single gas";
        is_mix = false;
    }
    return 0;
}


unsigned int db_class::prepare_bip()
{
    // if (!Is_database_open)
    //     return 2;

    if(!is_set_created)
        return 3;

    if(!is_mix)
        return 4;
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
    unsigned int le = front.length();
    front.erase(le-1,le);
    le = second.length();
    second.erase(le-1,le);

    querry = "select " + front + " from bip where gas_name in (" + second + ") order by id ASC";

    // for DEBUGGING    
    // std::cout<<"\n\n BIP querry : \n"<<querry<<"\n";

    // create a container
    std::size_t no_of_choices = gas_choice_id.size();
    std::vector<std::vector<float>> bip_arr(no_of_choices , std::vector<float>(no_of_choices));

    //open db
    int rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return 1;
    }
    //  make ready to querry db
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, querry.c_str(), -1, &stmt, NULL) == SQLITE_OK)
        for(unsigned int i=0; i<no_of_choices; i++){
            if(sqlite3_step(stmt) == 100)
                for(unsigned int j=0; j<no_of_choices; j++)
                    bip_arr[i][j] = (float)sqlite3_column_double(stmt, j);
            else
                return 2;
        }
    else
        return 2;

    // make a unique ptr for the data
    bip_pointer = std::make_unique<std::vector<std::vector<float>>>(bip_arr);

    // DEBUGGING print the bip data
    // for(const auto& i : bip_arr){
    //     for(const auto& j : i)
    //         std::cout<<j<<"\t";
    //     std::cout<<"\n";}
   
    sqlite3_close(db);
    return 0;
}

std::unique_ptr<std::vector<std::vector<float>>> db_class::get_bip_pointer()
{
    unsigned int res = prepare_bip();
    // std::cout<<"\n\nres : "<<res<<"\n\n";
    if(res==0)
        return std::move(bip_pointer);
    else
        return 0;
}

unsigned int db_class::cp_const_data_aquisition()
{
    // if (!Is_database_open)
    //     return 2;

    if(!is_set_created)
        return 3;

    std::vector<CP_Const> cp_const_data;
    cp_const_data.reserve(5);
    querry.erase();
    querry = "select * from c_ideal where gas_name=?";

    //open db
    int rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt;
    for(const auto& i : gas_choice_id){
        sqlite3_prepare_v2(db, querry.c_str(), -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, all_gas_names_map[i].c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == 100){
            CP_Const temp { (float)sqlite3_column_double(stmt, 2),
                            (float)sqlite3_column_double(stmt, 3),
                            (float)sqlite3_column_double(stmt, 4),
                            (float)sqlite3_column_double(stmt, 5)};
            cp_const_data.push_back(temp);
        sqlite3_finalize(stmt);
        }
        else{
            return 2;
            break;
        }

    }

    cp_const_vals = std::make_unique<std::vector<CP_Const>>(cp_const_data);
    
    // DEBUGGING
    // std::cout<<"\n";
    // for(const auto& i : cp_const_data){
    //     std::cout<<"\n"<<i.A<<"\t"<<i.B<<"\t"<<i.C<<"\t"<<i.D<<"\n";
    // }
    sqlite3_close(db);
    return 0;
}

std::unique_ptr<std::vector<CP_Const>> db_class::get_cp_const_pointer()
{
    unsigned int res = cp_const_data_aquisition();
    // std::cout<<"\n\nres : "<<res<<"\n\n";
    if(res==0)
        return std::move(cp_const_vals);
    else
        return 0;
}

unsigned int db_class::get_base_gas_props()
{
    if(!is_set_created)
        return 3;

    std::vector<base_props> base_gas_props;
    base_gas_props.reserve(5);


    // prepare query
    querry.erase();
    // create querry
    std::string second;
    for(const auto& i : gas_choice_id)
        second = second + "'" + all_gas_names_map[i] + "',";
    
    unsigned int le = second.length();
    second.erase(le-1,le);
    std::size_t no_of_choices = gas_choice_id.size();
    querry = "select tc,pc,w from base_gas_prop where gas_name in (" + second + ") order by id ASC";

    //open db
    int rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return 1;
    }

    //  make ready to querry db
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, querry.c_str(), -1, &stmt, NULL) == SQLITE_OK)
        for(unsigned int i=0; i<no_of_choices; i++){
            if(sqlite3_step(stmt) == 100){
                base_props temp { (float)sqlite3_column_double(stmt, 0),
                                  (float)sqlite3_column_double(stmt, 1),
                                  (float)sqlite3_column_double(stmt, 2)};
                base_gas_props.push_back(temp);
            }
                
            else
                return 2;
        }
    else
        return 2;

    // make a unique ptr for the data
    base_gas_props_ptr = std::make_unique<std::vector<base_props>>(base_gas_props);

    // DEBUGGING print the data
    // std::cout<<"\n";
    // for(const auto& i : base_gas_props)
    //     std::cout<<"\n"<<i.tc<<"\t"<<i.pc<<"\t"<<i.w<<"\n";
   
    sqlite3_close(db);
    return 0;
}

std::unique_ptr<std::vector<base_props>> db_class::get_base_gas_props_ptr()
{
     unsigned int res = get_base_gas_props();
    // std::cout<<"\n\nres : "<<res<<"\n\n";
    if(res==0)
        return std::move(base_gas_props_ptr);
    else
        return 0;
}