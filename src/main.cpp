#include "db_class.h"
#include <iostream>
int main()
{
    db_class mydbclass("props.db");
    unsigned int res = mydbclass.get_all_gas_names();
    //mydbclass.prepare_bip();
    std::unique_ptr<std::vector<std::vector<float>>> bip = mydbclass.get_bip_pointer();
    //bip = mydbclass.get_bip_pointer();

    mydbclass.cp_const_data_aquisition();

// print the bip data

    std::cout<<"\n I'm from main\n\n";
    if(bip!=NULL){
    for(auto row = bip->begin(); row != bip->end(); ++row){
        for(auto col = row->begin(); col != row->end(); ++col)
                std::cout<<*col<<"\t";
            std::cout<<"\n";}}

    std::cout<<"\n main done\n\n";

    return 0;
}