#include "db_class.h"
#include "pr_eos_class.h"
#include <iostream>
void testDB()
{
    db_class mydbclass("props.db");
    // unsigned int res = mydbclass.get_all_gas_names();
    // if(res==0)
    mydbclass.get_all_gas_names();
    mydbclass.choose_gas_from_user();

    std::unique_ptr<std::vector<std::vector<float>>> bip_ptr = mydbclass.get_bip_pointer();
    std::unique_ptr<std::vector<CP_Const>> cp = mydbclass.get_cp_const_pointer();
    std::shared_ptr<std::vector<base_props>> bp;
    bp = mydbclass.get_base_gas_props_ptr();

// print the bip data

    std::cout<<"\n binary interaction parameters \n";
    if(bip_ptr!=nullptr){
    for(auto row = bip_ptr->begin(); row != bip_ptr->end(); ++row){
        for(auto col = row->begin(); col != row->end(); ++col)
                std::cout<<*col<<"\t";
            std::cout<<"\n";}}
    std::cout<<"\n\n";

    std::cout<<"\n Cp parameters \n";
    if(cp!=nullptr)
        for(auto i = cp->begin(); i != cp->end(); ++i) 
            std::cout<<"\n"<<i->A<<"\t"<<i->B<<"\t"<<i->C<<"\t"<<i->D<<"\n";
    std::cout<<"\n\n";

    std::cout<<"\n base gas parameters \n";
    if(bp!=nullptr)
        for(auto i = bp->begin(); i != bp->end(); ++i) 
            std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\t"<<i->yi<<"\n";
    std::cout<<"\n\n";
}

void testPRclass()
{
    // float p, t;
    // std::cout<<"Enter Pressure in bar : ";
    // std::cin>>p;
    // std::cout<<"Enter temperature in celcius : ";
    // std::cin>>t;

    pr_eos eos(4, 40, "props.db");
    // eos.print_bip_data();
    eos.getZ(true);
    eos.print_base_data();
    eos.calc_dew();

}

int main()
{
    std::cout<<"\n I'm from main\n\n";
    // testDB();
    testPRclass();
    std::cout<<"\n\nEnd of main\n\n";
    return 0;
}
// one thing can be done open db again and again for each operations