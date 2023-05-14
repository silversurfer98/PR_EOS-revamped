#include "./include/db_class.h"
#include <iostream>
int main()
{
    db_class mydbclass("props.db");
    unsigned int res = mydbclass.get_all_gas_names();
    if(res==0)
        res = mydbclass.choose_gas_from_user();

    std::unique_ptr<std::vector<std::vector<float>>> bip = mydbclass.get_bip_pointer();
    std::unique_ptr<std::vector<CP_Const>> cp = mydbclass.get_cp_const_pointer();

// print the bip data

    std::cout<<"\n I'm from main\n\n";
    if(bip!=nullptr){
    for(auto row = bip->begin(); row != bip->end(); ++row){
        for(auto col = row->begin(); col != row->end(); ++col)
                std::cout<<*col<<"\t";
            std::cout<<"\n";}}
    // if(res == 0)
    if(cp!=nullptr)
        for(auto i = cp->begin(); i != cp->end(); ++i) 
            std::cout<<"\n"<<i->A<<"\t"<<i->B<<"\t"<<i->C<<"\t"<<i->D<<"\n";
    std::cout<<"\n main done\n\n";

    return 0;
}