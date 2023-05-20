#include "db_class.h"
#include "pr_eos_class.h"
#include <iostream>
int main()
{
    db_class mydbclass("props.db");
    // unsigned int res = mydbclass.get_all_gas_names();
    // if(res==0)
    mydbclass.get_all_gas_names();
    mydbclass.choose_gas_from_user();

    std::unique_ptr<std::vector<std::vector<float>>> bip_ptr = mydbclass.get_bip_pointer();
    std::unique_ptr<std::vector<CP_Const>> cp = mydbclass.get_cp_const_pointer();
    std::unique_ptr<std::vector<base_props>> bp;
    bp = mydbclass.get_base_gas_props_ptr();

// print the bip data

    std::cout<<"\n I'm from main\n\n";
    if(bip_ptr!=nullptr){
    for(auto row = bip_ptr->begin(); row != bip_ptr->end(); ++row){
        for(auto col = row->begin(); col != row->end(); ++col)
                std::cout<<*col<<"\t";
            std::cout<<"\n";}}
    // // if(res == 0)
    if(cp!=nullptr)
        for(auto i = cp->begin(); i != cp->end(); ++i) 
            std::cout<<"\n"<<i->A<<"\t"<<i->B<<"\t"<<i->C<<"\t"<<i->D<<"\n";

    if(bp!=nullptr)
        for(auto i = bp->begin(); i != bp->end(); ++i) 
            std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\n";

    std::cout<<"\n main done\n\n";

    pr_eos eos(2, 370, "props.db");
    eos.print_base();

    return 0;
}

int mmain()
{
    pr_eos eos(2, 370, "props.db");
    eos.print_base();
    return 0;
}
// one thing can be done open db again and again for each operations