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

float func(float value, std::shared_ptr<std::vector<float>> parameters)
{
    // f(Z) = Z3 - CZ2 + DZ - E = 0
    return pow(value, 3) - (*parameters)[0] * pow(value, 2) + (*parameters)[1] * value - (*parameters)[2];
}

float Derivative_of_func(float value, std::shared_ptr<std::vector<float>> parameters)
{
    // f(Z) = Z3 - CZ2 + DZ - E = 0
    // f'(Z) = 3Z2 - 2CZ + D = 0
    return 3 * pow(value, 2) - 2 * (*parameters)[0] * value + (*parameters)[1];
}


class pr_eos
{
private:
// private variable
    float p=1, t=273.15;
    std::unique_ptr<std::vector<base_props>> base_data_pt;
    std::unique_ptr<std::vector<std::vector<float>>> bip_data_ptr;
    PR_props pr_ans;
    unsigned int size_of_gas_data = 1;
    bool Is_mix = true;
    std::vector<PR_props> pr_data;

// private member funcs
    void pr_mix_report(PR_props* pr);
    void PR_consts_Calc(base_props* gas_prop, PR_props* prprops);

public:
// public variables
    db_class mydbclass;

// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void construct_pr_mix_props();
    void construct_pr_props();
    void PR_consts_Calc_mix();
};

pr_eos::pr_eos(float pressure, float temperature, const char* db_n) : mydbclass(db_n)
{
    p = pressure*0.1 - 0.101325;
    // p = 0.000001 * (pressure*100000 - 101325);
    t = temperature + 273.15;
    // db_class* mydbclass;
    // db_class mydbclass{db_name};
    // mydbclass = new db_class(db_name);
        unsigned int res = mydbclass.get_all_gas_names();
    if(res==0)
        res = mydbclass.choose_gas_from_user();
    
    size_of_gas_data = mydbclass.get_no_of_gases();
    std::cout<<"size of gase choice = "<<size_of_gas_data<<"\n\n";

    if(size_of_gas_data == 1)
        Is_mix = false;
    
    base_data_pt = mydbclass.get_base_gas_props_ptr();

    if(Is_mix)
        bip_data_ptr = mydbclass.get_bip_pointer();
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
    // pressure unit is Mpa
    prprops->k = 0.37464 + (1.54226 * gas_prop->w) - (0.26992 * pow(gas_prop->w ,2)); //the m-constant
    prprops->ac = (0.4572355289 * pow(r * gas_prop->tc, 2)) / (gas_prop->pc * 0.000001);  // preliminary PREOS a constant
    prprops->alpha = (1 + prprops->k * (1 - sqrt(t / gas_prop->tc))) * (1 + prprops->k * (1 - sqrt(t / gas_prop->tc)));

    prprops->a = prprops->ac * prprops->alpha;  //refined a-constant PREOS   cm6-pascal/mol2
    prprops->b = (0.077796074 * r * gas_prop->tc) / (gas_prop->pc * 0.000001); //b-constant PREOS   cm3/mol

    prprops->aa = prprops->a * p / (r * r * t* t);  //A constant fpr Z-equation
    prprops->bb = prprops->b * p / (r * t);  //B constant fpr Z-equation

    //constants for the equation 
    prprops->c = (1 - prprops->bb);
    prprops->d = (prprops->aa - 2 * prprops->bb - 3 * prprops->bb * prprops->bb);
    prprops->e = (prprops->aa * prprops->bb - prprops->bb * prprops->bb - prprops->bb * prprops->bb * prprops->bb);

}

void pr_eos::construct_pr_mix_props()
{
    // to implement, 
    // create pr_props vector
    // iterate through each base_gas_data and send to PR_const calc func
    pr_data.reserve(5);
    PR_props* ans = new PR_props;

    if(base_data_pt)
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            PR_consts_Calc(&(*i), ans);
            pr_data.push_back(*ans);
            
            // print data
            // pr_mix_report(&ans);
        }
        delete ans;
        // for(auto i : pr_data)
        //     pr_mix_report(&i);
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

    float t1, t2;

    // for(auto i : pr_data)
    //     std::cout << "\n m = " << i.k<<"\n";

    for (unsigned int i = 0; i < size_of_gas_data; i++)
    {
        for (unsigned int j = 0; j < size_of_gas_data; j++)
        {
            float a = (pr_data[i].a);
            float b = (pr_data[j].a);
            t1 = sqrt(a * b) * (1 - (*bip_data_ptr)[i][j]);
            // t1 = sqrt((pr_data[i].a) * (pr_data[j].a)) * (1 - (*bip_data_ptr)[i][j]);
            t2 = (*base_data_pt)[i].xi * (*base_data_pt)[j].xi * t1;
      
            // print matrix - DEBUG
            // std::cout<<t2<<"\t";

            pr_ans.a = pr_ans.a + t2;
        }
        // std::cout<<"\n";
    }

/*** range based for loop
    unsigned int a=0,b=0;
    for(auto i : pr_data){
        for(auto j : pr_data){
            t1 = sqrt(i.a * j.a) * (1 - (*bip_data_ptr)[a][b]);
            t2 = (*base_data_pt)[a].xi * (*base_data_pt)[b].xi * t1;
            pr_ans.a = pr_ans.a + t2;
            b++;
        }
        a++;
    }
***/

    for (unsigned int i = 0; i < size_of_gas_data; i++)
        pr_ans.b = pr_ans.b + (*base_data_pt)[i].xi * pr_data[i].b;


    // set these values to 0
    pr_ans.k = 0;
    pr_ans.ac = 0;
    pr_ans.alpha = 0;

    // these are the results to solve for Z
    pr_ans.aa = pr_ans.a * p / (r * r * t * t);  // A constant for Z-equation
    pr_ans.bb = pr_ans.b * p / (r * t);          // B constant for Z-equation

    pr_ans.c = (1 - pr_ans.bb);
    pr_ans.d = (pr_ans.aa - 2 * pr_ans.bb - 3 * pr_ans.bb * pr_ans.bb);
    pr_ans.e = (pr_ans.aa * pr_ans.bb - pr_ans.bb * pr_ans.bb - pr_ans.bb * pr_ans.bb * pr_ans.bb);

    std::cout<<"\n\nmix gas results ------> \n";
    pr_mix_report(&pr_ans);
    std::cout<<"\n----------------------> \n";
}

void pr_eos::construct_pr_props()
{
    if(Is_mix){
        construct_pr_mix_props();
        PR_consts_Calc_mix();
    }
    else{
        std::cout<<"\nsingle gas\n\n";
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i)
            PR_consts_Calc(&(*i), &pr_ans);
    std::cout<<"\n\nmix gas results ------> \n";
    pr_mix_report(&pr_ans);
    std::cout<<"\n----------------------> \n";
    }

    std::vector<float> parameters;
    parameters.reserve(3);

    // i think this is where the problem is ---------
    parameters.push_back(pr_ans.c);
    parameters.push_back(pr_ans.d);
    parameters.push_back(pr_ans.e);

    std::shared_ptr<std::vector<float>> ptr;
    ptr = std::make_shared<std::vector<float>>(parameters);

    float (*fun)(float, std::shared_ptr<std::vector<float>>) = func;
    float (*funcd)(float, std::shared_ptr<std::vector<float>>) = Derivative_of_func;

    // before solving anything lets find whether the equation has one or 3 real toots
    float Q1 = parameters[0]*parameters[1]/6 - parameters[2]/2 - pow(parameters[0],3)/27;
    float P1 = pow(parameters[0],2)/9 - parameters[1]/3;
    float D = Q1*Q1 - P1*P1*P1;

    if (D>=0){
        std::cout<<"\nonly one real root so going for newton raphson way\n";
        float f = 1.0;
        bool ans = newton_raphson_controlled(&f,fun,funcd,ptr,0.0001,50);
        std::cout<<"\nans : "<<f;
    }
    else{
        std::cout<<"\n\nhas three roots under cons\n\n";
       
        std::vector<float> initial;

        initial.push_back(0);
        initial.push_back(0.5);
        initial.push_back(1);

        std::shared_ptr<std::vector<float>> ini_p;
        ini_p = std::make_shared<std::vector<float>>(initial);

        weistrass_controlled(ini_p, fun, ptr, 0.01, 50);
        // if(weistrass_controlled(ini_p, fun, ptr, 0.001, 50)){
            std::cout<<"\nsuceeded\n";
            for(const auto& i : *ini_p)
                std::cout<<i<<"\n";
        // }

    }
    
    
}