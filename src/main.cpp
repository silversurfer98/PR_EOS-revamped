#include "db_class.h"
#include "pr_eos_class.h"
#include <iostream>
#include <limits>

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
    float p, t;
    std::cout<<"Enter Pressure in barA : ";
    std::cin>>p;
    std::cout<<"Enter temperature in celcius : ";
    std::cin>>t;
    printf("\nPressure : %f \t Temperature : %f\n\n",p,t);

    pr_eos eos(p, t, "props.db", true);
    // eos.set_print_debug_data(true);
    // eos.print_bip_data();
    // eos.getZ(true);
    // eos.get_cp(true);
    eos.calc_dew();
    eos.print_base_data();
    
}

void testPRclass_own()
{
    float p, t;
    std::cout<<"Enter Pressure in barA : ";
    std::cin>>p;
    std::cout<<"Enter temperature in celcius : ";
    std::cin>>t;
    printf("\nPressure : %f \t Temperature : %f\n\n",p,t);

    pr_eos eos(p, t, true);
    // eos.set_print_debug_data(true);
    // eos.getZ(true);
    // eos.get_cp(true);
    eos.calc_dew();
    eos.print_base_data();
    eos.print_bip_data();

    
}

void get_bip_test()
{
    // get that data
    uint16_t size_of_gas_data = 3;
    std::unique_ptr<std::vector<std::vector<float>>> bip_data_ptr;
    std::vector<std::vector<float>> bip_arr(size_of_gas_data , std::vector<float>(size_of_gas_data));
    for(uint16_t i=0; i<size_of_gas_data; i++)
    {
        for(uint16_t j=0; j<size_of_gas_data; j++)
        {
            if(bip_arr[i][j]!=0)
                continue;
            if(i==j)
            {
                bip_arr[i][j] = 0;
                continue;
            }
            else
            {
                std::cout<<"Enter the binary interaction parameter for gas"<<i+1<<" and gas"<<j+1<<" : ";
                std::cin>>bip_arr[i][j];
                bip_arr[j][i] = bip_arr[i][j];
            }
        }

    }

    bip_data_ptr = std::make_unique<std::vector<std::vector<float>>>(bip_arr);

    std::cout<<"\n binary interaction parameters \n";
    if(bip_data_ptr!=nullptr){
    for(auto row = bip_data_ptr->begin(); row != bip_data_ptr->end(); ++row){
        for(auto col = row->begin(); col != row->end(); ++col)
                std::cout<<*col<<"\t";
            std::cout<<"\n";}}
    std::cout<<"\n\n";
}

std::mutex pr_eos::pr_mix_data_lock;

int main()
{
    // std::cout<<"\n I'm from main\n\n";
    // testDB();
    // testPRclass_own();
    // get_bip_test();

    // std::cout<<"\n\nEnd of main\n\n";

    //true main
    std::cout<<"\n\n***************************************************************\n";
    std::cout<<"***********  Simple PR EOS property finder  *******************\n";
    std::cout<<"***************************************************************\n\n";
    
    float p, t;
    std::cout<<"Enter Pressure in barA : ";
    std::cin>>p;
    std::cout<<"Enter temperature in celcius : ";
    std::cin>>t;
    pr_eos eos(p, t, "props.db", true);

    std::cout<<"are u on ur own (y/N) : ";
    char ch;
    std::cin>>ch;
    if(ch=='Y' || ch == 'y')
        new(&eos) pr_eos(p, t, true);
    
    
    std::cout<<"\n\nDo u wann print debug data (y/N): ";
    std::cin>>ch;
    if(ch=='Y' || ch == 'y')
        eos.set_print_debug_data(true);

    uint16_t c;

    std::cout<<"\n\n\n Prints :\n1.Print Base gas Data\n2.Print BIP data\n\nEnter ur choice : ";
    std::cin>>c;
    std::cout<<"\n\n";
    switch (c)
    {
    case 1:
        eos.print_base_data();
        break;
    case 2:
       eos.print_bip_data();
        break;
    default:
        break;
    }


    std::cout<<"\n\n\n Abilities :\n1. Calc Z\n2. Calc Dew pt\n3. Calc Cp\n4.Print Base gas Data\n5.Print BIP data\n\nEnter ur choice : ";
    std::cin>>c;
    std::cout<<"\n\n";
    switch (c)
    {
    case 1:
        eos.getZ(true);
        break;
    case 2:
        eos.calc_dew();
        break;
    case 3:
        eos.get_cp(true);
        break;
    case 4:
        eos.print_base_data();
        break;
    case 5:
       eos.print_bip_data();
        break;
    default:
        std::cout<<"\n\n\n Varataaaaaaa ........................ \n\n\n";
        break;
    }
    
    std::cout<<"\n\n\n Varataaaaaaa ........................ \n\n\n";
    return 0;
}
// one thing can be done open db again and again for each operations