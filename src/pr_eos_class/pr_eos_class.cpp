#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>
#include<math.h>
#include "maths.h"
#include "db_class.h"

const float r = 8.3144598; // unit ---> J / K ⋅ mol

class pr_eos
{
private:
// private variable
    float p=1, t=273.15;
    const char* db_name;
    std::unique_ptr<std::vector<base_props>> base_data_pt;
    std::unique_ptr<std::vector<std::vector<float>>> bip_data_ptr;
    bool Is_mix = true;
    unsigned int size_of_gas_data = 1;
    PR_props pr_ans;

// private member funcs
    void pr_mix_report(PR_props* pr);
    void PR_consts_Calc(base_props* gas_prop, PR_props* prprops);
    void PR_consts_Calc_mix();

public:
// public variables
    db_class mydbclass;
    void construct_pr_props();
    std::vector<PR_props> pr_data;

// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();

};

pr_eos::pr_eos(float pressure, float temperature, const char* db_n) : mydbclass(db_n)
{
    p = pressure;
    t = temperature;
    db_name = db_n;
    // db_class* mydbclass;
    // db_class mydbclass{db_name};
    // mydbclass = new db_class(db_name);
    unsigned int res = mydbclass.get_all_gas_names();
    if(res==0)
        res = mydbclass.choose_gas_from_user();
    
    Is_mix = mydbclass.is_mix;
    // std::cout<<"\n\n res = "<<res<<"\n\n";
    base_data_pt = mydbclass.get_base_gas_props_ptr();

    if(Is_mix){
        bip_data_ptr = mydbclass.get_bip_pointer();
        size_of_gas_data = mydbclass.size_of_gas_data;
    }
}

pr_eos::~pr_eos()
{
    std::cout<<"\n\nPR_EOS destructor has been called\n\n";
}

void pr_eos::print_base_data()
{
    std::cout<<"\n"<<"Tc"<<"\t"<<"Pc"<<"\t"<<"Acc Factor"<<"\n";
    if(base_data_pt)
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i) 
            std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\n";
}

void pr_eos::print_bip_data()
{
    std::cout<<"\n\n";
    if(Is_mix)
        if(bip_data_ptr){
            for(auto row = bip_data_ptr->begin(); row != bip_data_ptr->end(); ++row){
            for(auto col = row->begin(); col != row->end(); ++col)
                    std::cout<<*col<<"\t";
                std::cout<<"\n";}}
    else
        std::cout<<"\nenakune varuveengala da\n";

}

void pr_eos::PR_consts_Calc(base_props* gas_prop, PR_props* prprops)
{
    prprops->k = 0.37464 + 1.54226 * gas_prop->w - 0.26992 * gas_prop->w * gas_prop->w; //the m-constant
    prprops->ac = (0.45723553 * r * r * gas_prop->tc * gas_prop->tc) / gas_prop->pc;  // preliminary PREOS a constant
    prprops->alpha = (1 + prprops->k * (1 - sqrt(t/ gas_prop->tc))) * (1 + prprops->k * (1 - sqrt(t/ gas_prop->tc)));

    prprops->a = prprops->ac * prprops->alpha;  //refined a-constant PREOS   cm6-pascal/mol2
    prprops->b = (0.077796074 * r * gas_prop->tc) / gas_prop->pc; //b-constant PREOS   cm3/mol

    prprops->aa = prprops->a * p / (r * r * t* t);  //A constant fpr Z-equation
    prprops->bb = prprops->b * p / (r * t);  //B constant fpr Z-equation

    //constants for the equation 
    prprops->c = (1 - prprops->bb);
    prprops->d = (prprops->aa - 2 * prprops->bb - 3 * prprops->bb * prprops->bb);
    prprops->e = (prprops->aa * prprops->bb - prprops->bb * prprops->bb - prprops->bb * prprops->bb * prprops->bb);

}

void pr_eos::construct_pr_props()
{
    // to implement, 
    // create pr_props vector
    // iterate through each base_gas_data and send to PR_const calc func
    pr_data.reserve(5);
    if(base_data_pt)
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            PR_props ans;
            PR_consts_Calc(&(*i), &ans);
            pr_data.push_back(ans);
            pr_mix_report(&ans);
        }
}

void pr_eos::pr_mix_report(PR_props* pr)
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

void pr_eos::PR_consts_Calc_mix()
{
// only logic is here dont compile

/***
    for (unsigned int i = 0; i < 10; i++)
        for (unsigned int j = 0; j < 10; j++)
            aij[i][j] = sqrt(prmix[i].a * prmix[j].a) * (1 - binaryint_parameters[i][j]);

    for (unsigned int i = 0; i < 10; i++)
        for (unsigned int j = 0; j < 10; j++)
            axij[i][j] = x[i] * x[j] * (aij[i][j]);

    for (unsigned int i = 0; i < 10; i++)
        bi[i] = x[i] * prmix[i].b;


    for (unsigned int i = 0; i < 10; i++)
        for (unsigned int j = 0; j < 10; j++)
            pr_ans.a = pr_ans.a + axij[i][j];

    for (unsigned int i = 0; i < 10; i++)
        pr_ans.b = pr_ans.b + bi[i];

    pr_ans.aa = pr_ans.a * gas1[0].p / (r * r * gas1[0].t * gas1[0].t);  //A constant fpr Z-equation
    pr_ans.bb = pr_ans.b * gas1[0].p / (r * gas1[0].t);  //B constant fpr Z-equation
***/
    pr_ans.c = (1 - pr_ans.bb);
    pr_ans.d = (pr_ans.aa - 2 * pr_ans.bb - 3 * pr_ans.bb * pr_ans.bb);
    pr_ans.e = (pr_ans.aa * pr_ans.bb - pr_ans.bb * pr_ans.bb - pr_ans.bb * pr_ans.bb * pr_ans.bb);

    std::cout<<"mix gas results ------> \n";
    pr_mix_report(&pr_ans);
    std::cout<<"\n----------------------> \n";
}