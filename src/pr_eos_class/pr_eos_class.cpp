#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>
#include<math.h>
#include "maths.h"
#include "db_class.h"

//PR EOS properties definition
struct PR_props
{
    float a, b, k, alpha, ac;
};

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

void swap(float* a, float* b)
{
    float t;
    t = *a;
    *a = *b;
    *b = t;
}


class pr_eos
{
private:
// private variable
    std::vector<PR_props> pr_mix_data;
    std::vector<float> parameters;
    std::unique_ptr<std::vector<std::vector<float>>> bip_data_ptr;
    unsigned int size_of_gas_data = 1;
    bool Is_mix = true;
    float aa, bb, a, b;
    bool cal_phi = false;
    std::vector<float> aik;
    uint16_t no_of_root = 0;
    // for dew pt calc
    std::unique_ptr<std::vector<float>> phi_V_ptr;
    std::unique_ptr<std::vector<float>> phi_L_ptr;
    float estimated_temp = 0;
    float *xi_norm;
    bool should_I_use_yi = true;

    std::shared_ptr<std::vector<float>> ini_p;


// private member funcs
    void pr_mix_report(PR_props* pr);
    void PR_consts_Calc(base_props* gas_prop, PR_props* prprops);
    void pr_mix_props();
    void pr_single_gas_props();
    void calc_phi(float z, std::unique_ptr<std::vector<float>>& phi);
    float dew_pt_calc(bool is_first_time);


public:
// public variables
    std::unique_ptr<std::vector<base_props>> base_data_pt;
    float z, zl;
    float p, t;
    db_class mydbclass;

// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void getZ(bool Calc_phi);
    void calc_dew();
};

pr_eos::pr_eos(float pressure, float temperature, const char* db_n) : mydbclass(db_n)
{
    p = pressure*0.1 - 0.101325;
    // p = 0.000001 * (pressure*100000 - 101325);
    t = temperature + 273.15;
    // mydbclass = new db_class(db_n);
    
    z = 1; zl = 0;
    parameters.reserve(3);
    for(uint16_t i = 0;i<3;i++)
        parameters.push_back(1.0);
    estimated_temp = 0.0;
 
    unsigned int res = mydbclass.get_all_gas_names();
    if(res==0)
        res = mydbclass.choose_gas_from_user();
    
    size_of_gas_data = mydbclass.get_no_of_gases();
    std::cout<<"size of gase choice = "<<size_of_gas_data<<"\n\n";

    if(size_of_gas_data == 1)
        Is_mix = false;
    
    base_data_pt = mydbclass.get_base_gas_props_ptr();

    if(Is_mix){
        bip_data_ptr = mydbclass.get_bip_pointer();
        pr_mix_data.resize(size_of_gas_data);

        ini_p = std::make_shared<std::vector<float>>();
        ini_p->resize(3);
        xi_norm = new float[size_of_gas_data];
    }
    std::cout<<"PR class constructor called and finished \n\n";
}

pr_eos::~pr_eos()
{
    delete[] xi_norm;
    std::cout<<"\n\nPR_EOS destructor has been called\n\n";
}

// print funcs
void pr_eos::print_base_data()
{
    std::cout<<"\n\n";
    if(base_data_pt){
        std::cout<<"\n"<<"Tc"<<"\t"<<"Pc"<<"\t"<<"Acc factor"<<"\t"<<"Yi"<<"\t"<<"Xi"<<"\t"<<"Tsat"<<"\n";
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i) 
            std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\t"<<i->yi<<"\t"<<i->xi<<"\t"<<i->tsat<<"\n";
    }
    else
        std::cout<<"\nenakune varuveengala da\n";
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

void pr_eos::pr_mix_report(PR_props* pr)
{
    std::cout << "\n m = " << pr->k;
    std::cout << "\n ac = " << pr->ac;
    std::cout << "\n alpha = " << pr->alpha;
    std::cout << "\n a = " << pr->a;
    std::cout << "\n b = " << pr->b;
}

//-------------------------- end of print funcs -----------------------------------

//core PR EOS func contains all math
void pr_eos::PR_consts_Calc(base_props* gas_prop, PR_props* prprops)
{
    // pressure unit is Mpa
    prprops->k = 0.37464 + (1.54226 * gas_prop->w) - (0.26992 * pow(gas_prop->w ,2)); //the m-constant
    prprops->ac = (0.4572355289 * pow(r * gas_prop->tc, 2)) / (gas_prop->pc * 0.000001);  // preliminary PREOS a constant
    prprops->alpha = (1 + prprops->k * (1 - sqrt(t / gas_prop->tc))) * (1 + prprops->k * (1 - sqrt(t / gas_prop->tc)));

    prprops->a = prprops->ac * prprops->alpha;  //refined a-constant PREOS   cm6-pascal/mol2
    prprops->b = (0.077796074 * r * gas_prop->tc) / (gas_prop->pc * 0.000001); //b-constant PREOS   cm3/mol

    // prprops->aa = prprops->a * p / (r * r * t* t);  //A constant fpr Z-equation
    // prprops->bb = prprops->b * p / (r * t);  //B constant fpr Z-equation

    // constants for the equation 
    // prprops->c = (1 - prprops->bb);
    // prprops->d = (prprops->aa - 2 * prprops->bb - 3 * prprops->bb * prprops->bb);
    // prprops->e = (prprops->aa * prprops->bb - prprops->bb * prprops->bb - prprops->bb * prprops->bb * prprops->bb);
    gas_prop->tsat = gas_prop->tc / (1 - 3 * log(p / (gas_prop->pc * 0.000001)) / (16.11809565095832 * (1 + gas_prop->w)));

}


//all other shenanigans here

// new WTF
void pr_eos::pr_single_gas_props()
{
    PR_props* ans = new PR_props;
    std::cout<<"\nsingle gas\n\n";
    for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i)
        PR_consts_Calc(&(*i), ans);
    
    aa = 0; bb = 0; a = ans->a; b = ans->b;
    // these are the results to solve for Z
    aa = a * p / (r * r * t * t);  // A constant for Z-equation
    bb = b * p / (r * t);          // B constant for Z-equation
    
    delete ans;

    // float c,d,e;
    // c = (1 - bb);
    // d = (aa - 2 * bb - 3 * pow(bb, 2));
    // e = (aa * bb - pow(bb, 2) - pow(bb, 3));
    // printf("\n\nC : %f\nD : %f\nE : %f\n\n",c,d,e);

    parameters[0] = ((1 - bb));
    parameters[1] = ((aa - 2 * bb - 3 * pow(bb, 2)));
    parameters[2] = ((aa * bb - pow(bb, 2) - pow(bb, 3)));
}

void pr_eos::pr_mix_props()
{
// to implement, 
    // create pr_props vector
    // iterate through each base_gas_data and send to PR_const calc func
    // pr_mix_data.reserve(size_of_gas_data);
    PR_props* ans = new PR_props;

    uint16_t k = 0;
    if(base_data_pt)
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            PR_consts_Calc(&(*i), ans);
            // pr_mix_data.push_back(*ans);
            pr_mix_data.at(k) = *ans;
            k++;
        }
    delete ans;

    // for printing
        // for(auto i : pr_mix_data)
        //     pr_mix_report(&i);

    if(cal_phi){
        aik.resize(size_of_gas_data);
        std::fill(aik.begin(), aik.end(),0.0);
    }

    float aij, axij, x, y;
    aa = 0; bb = 0; a = 0; b = 0;
    for (unsigned int i = 0; i < size_of_gas_data; i++)
    {
        x = (pr_mix_data[i].a);
        for (unsigned int j = 0; j < size_of_gas_data; j++)
        {
            y = (pr_mix_data[j].a);
            aij = sqrt(x * y) * (1 - (*bip_data_ptr)[i][j]);

            if(should_I_use_yi)
                axij = (*base_data_pt)[i].yi * (*base_data_pt)[j].yi * aij;
            else
                axij = (*base_data_pt)[i].xi * (*base_data_pt)[j].xi * aij;
        
            // print matrix - DEBUG
            // std::cout<<t2<<"\t";
        
            a = a + axij;

            if(cal_phi){
                if(should_I_use_yi)
                    aik[j] = aik[j] + (*base_data_pt)[i].yi * aij;
                else
                    aik[j] = aik[j] + (*base_data_pt)[i].xi * aij;
            }
                
        }
        // std::cout<<"\n";
    }

    for (unsigned int i = 0; i < size_of_gas_data; i++){
        if(should_I_use_yi)
            b = b + ((*base_data_pt)[i].yi * pr_mix_data[i].b);
        else
            b = b + ((*base_data_pt)[i].xi * pr_mix_data[i].b);
    }

    // these are the results to solve for Z
    aa = a * p / (r * r * t * t);  // A constant for Z-equation
    bb = b * p / (r * t);          // B constant for Z-equation

    // float c,d,e;
    // c = (1 - bb);
    // d = (aa - 2 * bb - 3 * pow(bb, 2));
    // e = (aa * bb - pow(bb, 2) - pow(bb, 3));
    // printf("\n\nC : %f\nD : %f\nE : %f\n\n",c,d,e);

    parameters[0] = ((1 - bb));
    parameters[1] = ((aa - 2 * bb - 3 * pow(bb, 2)));
    parameters[2] = ((aa * bb - pow(bb, 2) - pow(bb, 3)));
}
// --------------------------------
void pr_eos::getZ(bool Calc_phi)
{
    cal_phi = Calc_phi;
    if(Is_mix)
        pr_mix_props();
    else
        pr_single_gas_props();

    // create ptr to parameters
    std::shared_ptr<std::vector<float>> ptr = std::make_shared<std::vector<float>>(parameters);

    // create function pointers
    float (*fun)(float, std::shared_ptr<std::vector<float>>) = func;
    float (*funcd)(float, std::shared_ptr<std::vector<float>>) = Derivative_of_func;

    // Print Parameters DEBUG
    // for(const auto& i : parameters)
        // std::cout<<"\n"<<i;

    // before solving anything lets find whether the equation has one or 3 real toots
    float Q1 = parameters[0]*parameters[1]/6 - parameters[2]/2 - pow(parameters[0],3)/27;
    float P1 = pow(parameters[0],2)/9 - parameters[1]/3;
    float D = Q1*Q1 - P1*P1*P1;

    if (D>=0){
        no_of_root = 1;
        std::cout<<"\nonly one real root so going for newton raphson way\n";
        float f = 1;
        if(newton_raphson_controlled(&f,fun,funcd,ptr,0.0001,50))
            z = f;
        std::cout<<"\nans : "<<z;
    }
    else{
        no_of_root = 3;
        std::cout<<"\nhas three roots - weistrass\n";

        // std::vector<float> ini;
        // ini.reserve(3);

        // ini.push_back(0);
        // ini.push_back(0.5);
        // ini.push_back(1);

        // std::shared_ptr<std::vector<float>> ini_p = std::make_shared<std::vector<float>>(ini);

        ini_p->at(0) = 0; 
        ini_p->at(1) = 0.5; 
        ini_p->at(2) = 1; 

        // for weistrass initial guess is VERY VERY IMPORTANT -------
        if(weistrass_controlled(ini_p, fun, ptr, 0.001, 50)){
            std::cout<<"\nweistrass suceeded\n";
            // alternative to qsort cause only 3 no.s
            if ((*ini_p)[0] > (*ini_p)[2])
                swap(&(*ini_p)[0], &(*ini_p)[2]);

            if ((*ini_p)[0] > (*ini_p)[1])
                swap(&(*ini_p)[0], &(*ini_p)[1]);

            if ((*ini_p)[1] > (*ini_p)[2])
                swap(&(*ini_p)[1], &(*ini_p)[2]);
            // -----------------------------------------
            z = (*ini_p)[2];
            zl = (*ini_p)[0];
        }
        std::cout<<"\nans : "<<z<<"\t"<<zl;
        
    }
}
// ------------------------------- dew pt calc --------------------
void pr_eos::calc_phi(float Z, std::unique_ptr<std::vector<float>>& phi)
{
    // totally under construction
    // printf("\n vera maari vera maari \n");
    float temp1 = 0;
    for(uint16_t i=0;i<size_of_gas_data;i++)
    {
        temp1 = pr_mix_data[i].b / b;

        // if(Z!=0 && (*base_data_pt)[i].yi!=0)
        if(Z!=0)
            phi->at(i) = exp(((temp1) * (Z - 1)) - 
                            (log(Z - bb)) -
                            (0.35355339059327373 * aa / bb) * 
                            ((2 * aik[i] / a) - temp1) *
                            log((Z + 2.414213562373095*bb) / (Z - 0.41421356237309515*bb)));

    }
}

float pr_eos::dew_pt_calc(bool is_first_time)
{
    printf("\n vera maari vera maari \n");

    phi_V_ptr = std::make_unique<std::vector<float>>();
    phi_V_ptr->resize(size_of_gas_data);
    std::fill(phi_V_ptr->begin(), phi_V_ptr->end(), 0);
    calc_phi(z, phi_V_ptr);

    printf("\n Phi vapour values \n");
    for(const auto i : *phi_V_ptr)
        std::cout<<i<<"\n";
    
    uint16_t j = 0;
    float xi_total = 0;
    // for (auto& i : *base_data_pt){
    for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
        i->xi = i->yi / exp(log(i->pc * 0.000001 / p) + (5.37269855031944 * (1 + i->w) * (1 - (i->tc / t))));
        // std::cout<<"\nKi = "<<exp(log(i->pc * 0.000001 / p) + (5.37269855031944 * (1 + i->w) * (1 - (i->tc / t))));
        // i->xi = 1;
        xi_total = xi_total + i->xi;
        j++;
    }
    print_base_data();

    // find xi_initial
    j=0;
    if(is_first_time)
        // for (const auto i : *base_data_pt){
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            // xi_norm[j] = (i.xi / xi_total);
            i->xi = (i->xi / xi_total);
            estimated_temp = estimated_temp + i->tsat * i->xi;
            j++;
        }


    // find the phi l
    phi_L_ptr = std::make_unique<std::vector<float>>();
    phi_L_ptr->resize(size_of_gas_data);
    std::fill(phi_L_ptr->begin(), phi_L_ptr->end(), 0);

    float xi_total_tolerance = 1e-06, xi_total_new = 0;
    // loop to refine xi
    for (uint32_t i = 0; i < 20; i++){
        j=0;
        should_I_use_yi = false;
        // pr_mix_data.clear();
        getZ(cal_phi);
        if(zl<=0)
            break;

        calc_phi(zl,phi_L_ptr);

        printf("\n Phi liquid values at %dth iteration \n", i);
        for(const auto i : *phi_L_ptr)
            std::cout<<i<<"\n";

        // update xi using the yi, phi_v and phi_l
        j=0;
        // for(auto i : *base_data_pt){
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            i->xi = i->yi * (phi_V_ptr->at(j) / phi_L_ptr->at(j));
            xi_total_new = xi_total_new + i->xi;
            j++;
        }

        j = 0;
        // for(auto i : *base_data_pt){
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            i->xi = i->xi / xi_total_new;
            j++;
        }

        print_base_data();

        if(abs(xi_total_new - xi_total) <= xi_total_tolerance)
            break;
        
        xi_total = xi_total_new;

    }

    return xi_total - 1;
}

void pr_eos::calc_dew()
{
    float t2 = 0, t1 = 0, fx = 0, tnew = 0;

    dew_pt_calc(true);
    printf("\n\nestimated temp is %f\n\n",estimated_temp);

    t1 = estimated_temp - 25.0;
    t2 = estimated_temp + 25.0;
    
    //secant
    // uint16_t iters = 25;
    // float tn = 0, ft, ftt, tolerance=1e-03;
    // for (uint16_t i = 0; i < iters; i++)
    // {
    //     t = t1;
    //     ft = dew_pt_calc(false);
    //     t = t2;
    //     ftt = dew_pt_calc(false);
    //     float temp = (ft - ftt);
    //     if (abs(temp) <= tolerance)
    //     {
    //         std::cout << "\n\nomale converged at "<<i<<" na idhukku mela pova maaten \n\n";
    //         break;
    //     }
    //     tn = t1 - ft * ((t1 - t2) / (ft - ftt));

    //     t1 = t2;
    //     t2 = tn;
        
    //     std::cout << "\n\nat the end of every secant loop --> " << tn << "\n\n";
    // }

}