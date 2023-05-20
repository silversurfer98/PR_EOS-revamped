#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>
#include "maths.h"
#include "db_class.h"

// data structs


class pr_eos
{
private:
// private variable
    // std::vector<base_props> base_data;

    float p=1,t=273.15;
// private member funcs
    void pr_mix_report(PR_props* pr);

public:
// public variables
// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    void print_base();

};

pr_eos::pr_eos(float pressure, float temperature, const char* db_name)
{
    p = pressure;
    t = temperature;

    db_class mydbclass(db_name);
    // unsigned int res = mydbclass.get_all_gas_names();
    // if(res==0)
        // mydbclass.choose_gas_from_user();

    mydbclass.get_all_gas_names();
    mydbclass.choose_gas_from_user();
    
    // bp = std::make_unique<std::vector<base_props>>(base_data);

    // std::unique_ptr<std::vector<base_props>>
    std::unique_ptr<std::vector<base_props>> base_data;
    base_data = mydbclass.get_base_gas_props_ptr();
    if(base_data!=nullptr)
        for(auto i = base_data->begin(); i != base_data->end(); ++i) 
            std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\n";


    // base_data = mydbclass.get_base_gas_props_ptr();

}

void pr_eos::print_base()
{
    std::cout<<"omale";
    // if(base_data!=nullptr)
        // for(auto i = base_data->begin(); i != base_data->end(); ++i) 
            // std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\n";
}

void pr_mix_report(PR_props* pr)
{
    std::cout << "\n m = " << pr->k;
    std::cout << "\n ac = " << pr->ac;
    std::cout << "\n alpha = " << pr->alpha;
    std::cout << "\n a = " << pr->a;
    std::cout << "\n b = " << pr->b;
    std::cout << "\n A = " << pr->aa;
    std::cout << "\n B = " << pr->bb;
    std::cout << "\n C = " << pr->c;
    std::cout << "\n D = " << pr->d;
    std::cout << "\n E = " << pr->e << "\n";
}