#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>
#include<math.h>
#include "maths.h"
#include "db_class.h"

#include <iomanip>

//PR EOS properties definition
struct PR_props
{
    float a, b, k, alpha, ac;
};

const float r = 8.3144598; // unit ---> J / K ⋅ mol

float func(float value, std::shared_ptr<std::vector<float>> parameters)
{
    // f(Z) = Z3 - CZ2 + DZ - E = 0
    return pow(value, 3) + (*parameters)[0] * pow(value, 2) + (*parameters)[1] * value + (*parameters)[2];
}

float Derivative_of_func(float value, std::shared_ptr<std::vector<float>> parameters)
{
    // f(Z) = Z3 - CZ2 + DZ - E = 0
    // f'(Z) = 3Z2 - 2CZ + D = 0
    return 3 * pow(value, 2) + 2 * (*parameters)[0] * value + (*parameters)[1];
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
    float *xi_not_norm;
    bool should_I_use_yi = true;
    float t_reserve;

    std::shared_ptr<std::vector<float>> ini_p;


// private member funcs
    void pr_mix_report(PR_props* pr);
    void PR_consts_Calc(base_props* gas_prop, PR_props* prprops);
    void pr_mix_props();
    void pr_single_gas_props();
    void calc_phi(float z, std::unique_ptr<std::vector<float>>& phi);
    float dew_pt_calc(bool is_first_time);
    void calc_cp();
    


public:
// public variables
    std::unique_ptr<std::vector<base_props>> base_data_pt;
    float z, zl;
    float p, t;
    float cp, cv, cp_ig, cv_ig, cp_r, cv_r;
    //config
    bool use_trig_method                = false;
    float root_precision                = 1e-05;
    uint16_t max_root_find_iterations   = 50;
    float xi_total_tolerance            = 1e-06;
    bool print_debug_data               = false;
    db_class mydbclass;

// public member funcs
    pr_eos() { };
    pr_eos(float pressure, float temperature, const char* db_name, bool Calc_phi);
    pr_eos(float pressure, float temperature, bool Calc_phi);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void getZ(bool print_z);
    void calc_dew();
    void get_cp(bool print_val);

    //config
    void set_use_trig_method(bool use_trig_method_f);
    void set_print_debug_data(bool print_debug_data_f);
    void set_max_root_find_iterations(uint16_t max_root_find_iterations_f);
    void set_root_precision(float root_precision_f);
    void set_xi_total_tolerance(float xi_total_tolerance_f);
};

pr_eos::pr_eos(float pressure, float temperature, const char* db_n, bool Calc_phi) : mydbclass{db_n}
{
    // p = pressure*0.1 - 0.101325;
    p = pressure*0.1;
    t = temperature + 273.15;
    t_reserve = t;

    cal_phi = Calc_phi;
    z = 1; zl = 0;

    parameters.reserve(3);
    for(uint16_t i = 0;i<3;i++)
        parameters.push_back(1.0);
 
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
        if(cal_phi){
            estimated_temp = 0.0;
            xi_not_norm = new float[size_of_gas_data];

            phi_V_ptr = std::make_unique<std::vector<float>>();
            phi_V_ptr->resize(size_of_gas_data);

            phi_L_ptr = std::make_unique<std::vector<float>>();
            phi_L_ptr->resize(size_of_gas_data);
        }
    }
    if(print_debug_data)
        std::cout<<"PR class constructor called and finished \n\n";

    use_trig_method = false;
    root_precision = 1e-05;
    max_root_find_iterations = 50;
    xi_total_tolerance = 1e-06;
    print_debug_data = false;
}

//overloaded constructor

pr_eos::pr_eos(float pressure, float temperature, bool Calc_phi) //: mydbclass()
{
    p = pressure*0.1;
    t = temperature + 273.15;
    t_reserve = t;

    cal_phi = Calc_phi;
    z = 1; zl = 0;

    parameters.reserve(3);
    for(uint16_t i = 0;i<3;i++)
        parameters.push_back(1.0);
    
    std::cout<<"How many gases are u gonna create : ";
    std::cin>>size_of_gas_data;

    if(size_of_gas_data == 1)
        Is_mix = false;

    std::cout<<"\n\n Enter base gas data : \n\n";
    base_data_pt = std::make_unique<std::vector<base_props>>();
    base_data_pt->resize(size_of_gas_data);

    uint16_t j = 0;
    for(auto i : *base_data_pt){
        std::cout<<"Enter base gas data for gas"<<j+1<<"\n";
        // tc, pc, w, yi, xi, tsat
        std::cout<<"Enter Tc [in K]= ";
        std::cin>>i.tc;
        
        std::cout<<"Enter Pc [in Pa]= ";
        std::cin>>i.pc;

        std::cout<<"Enter w = ";
        std::cin>>i.w;

        std::cout<<"Enter Yi= ";
        std::cin>>i.yi;

        i.xi = 0;
        i.tsat = 0;
    }

    // get bip data
    if(Is_mix){
        pr_mix_data.resize(size_of_gas_data);

        // get that data
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


        ini_p = std::make_shared<std::vector<float>>();
        ini_p->resize(3);
        if(cal_phi){
            estimated_temp = 0.0;
            xi_not_norm = new float[size_of_gas_data];

            phi_V_ptr = std::make_unique<std::vector<float>>();
            phi_V_ptr->resize(size_of_gas_data);

            phi_L_ptr = std::make_unique<std::vector<float>>();
            phi_L_ptr->resize(size_of_gas_data);
        }
    }
    if(print_debug_data)
        std::cout<<"PR class overloaded constructor called and finished \n\n";

    use_trig_method = false;
    root_precision = 1e-05;
    max_root_find_iterations = 50;
    xi_total_tolerance = 1e-06;
    print_debug_data = false;
}

//-----------------------------------------------------------------------------------------------------

pr_eos::~pr_eos()
{
    delete[] xi_not_norm;
    if(print_debug_data)
        std::cout<<"\n\nPR_EOS destructor has been called\n\n";
}

//config
void pr_eos::set_use_trig_method(bool use_trig_method_f)
{
    use_trig_method = use_trig_method_f;   
}
void pr_eos::set_print_debug_data(bool print_debug_data_f)
{
    print_debug_data= print_debug_data_f;   
}
void pr_eos::set_max_root_find_iterations(uint16_t max_root_find_iterations_f)
{
    max_root_find_iterations = max_root_find_iterations_f;   
}
void pr_eos::set_root_precision(float root_precision_f)
{
    root_precision = root_precision_f;   
}
void pr_eos::set_xi_total_tolerance(float xi_total_tolerance_f)
{
    xi_total_tolerance = xi_total_tolerance_f;   
}

// print funcs
void pr_eos::print_base_data()
{
    std::cout<<"\n\n";
    if(base_data_pt)
    {
        std::cout<< std::left << std::setw(8) <<"Tc"<< std::left << std::setw(15) <<"Pc"<< std::left << std::setw(15) <<"Acc factor"
                 << std::left << std::setw(5) <<"Yi"<< std::left << std::setw(15) <<"Xi"<< std::left << std::setw(8) <<"Tsat"<<std::endl;

        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i) 
            std::cout<< std::left << std::setw(8) <<i->tc<< std::left << std::setw(15) <<i->pc<< std::left << std::setw(15) <<i->w
                     << std::left << std::setw(5) <<i->yi<< std::left << std::setw(15) <<i->xi<< std::left << std::setw(8) <<i->tsat<<std::endl;
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
    
    // DEBUG code
    // float c,d,e;
    // c = (1 - bb);
    // d = (aa - 2 * bb - 3 * pow(bb, 2));
    // e = (aa * bb - pow(bb, 2) - pow(bb, 3));
    // printf("\n\nC : %f\nD : %f\nE : %f\n\n",c,d,e);

    parameters[0] = ((bb - 1));
    parameters[1] = ((aa - 2 * bb - 3 * pow(bb, 2)));
    parameters[2] = ((-1 * aa * bb + pow(bb, 2) + pow(bb, 3)));
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

    // DEBUG code
    // float c,d,e;
    // c = (1 - bb);
    // d = (aa - 2 * bb - 3 * pow(bb, 2));
    // e = (aa * bb - pow(bb, 2) - pow(bb, 3));
    // printf("\n\nC : %f\nD : %f\nE : %f\n\n",c,d,e);

    parameters[0] = ((bb - 1));
    parameters[1] = ((aa - 2 * bb - 3 * pow(bb, 2)));
    parameters[2] = ((-1 * aa * bb + pow(bb, 2) + pow(bb, 3)));
}

void pr_eos::getZ(bool print_z)
{
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
    if(print_debug_data){
        std::cout<<"C2 \t C1 \t C0\n";
        for(const auto& i : parameters)
            std::cout<<i<<"\t";
    }
    

    // before solving anything lets find whether the equation has one or 3 real toots
    float Q1 = parameters[0]*parameters[1]/6 - parameters[2]/2 - pow(parameters[0],3)/27;
    float P1 = pow(parameters[0],2)/9 - parameters[1]/3;
    float D = Q1*Q1 - P1*P1*P1;

    if (D>=0){
        no_of_root = 1;

        if(use_trig_method)
        {
            // proposed way to find root in literature;
            if(print_debug_data)
                std::cout<<"\nUsing trig method\n";
            z = pow((Q1 + sqrt(D)),1/3) + pow((Q1 - sqrt(D)),1/3) - parameters[0]/3;
        }

        else{
            if(print_debug_data)
                std::cout<<"\nonly one real root so going for newton raphson way\n";

        float f = 1;
        if(newton_raphson_controlled(&f,fun,funcd,ptr,root_precision,max_root_find_iterations))
            z = f;

            if(print_debug_data)
                std::cout<<"\nans : "<<z;
        }
    }
    else{
        no_of_root = 3;
        if(print_debug_data)
            std::cout<<"\nhas three roots - weistrass\n";
        
        if(use_trig_method)
        {
            // proposed way to find root in literature;
            if(print_debug_data)
                std::cout<<"\nUsing trig method\n";
            // proposed way to find root in literature;
            float t1 = Q1*Q1 / (P1*P1*P1);
            float t2 = sqrt(1 - t1) / sqrt(t1) * Q1 / abs(Q1);
            float theta = atan(t2);
            if(theta<0)
                theta = theta + M_PI;
            
            ini_p->at(0) = 2*sqrt(P1) *  cos(theta / 3) - parameters[0]/3;
            ini_p->at(1) = 2*sqrt(P1) * cos((theta + 2*M_PI)/3) - parameters[0]/3;
            ini_p->at(2) = 2*sqrt(P1) * cos((theta + 4*M_PI)/3) - parameters[0]/3;
        }

        else{
        ini_p->at(0) = 0; 
        ini_p->at(1) = 0.5; 
        ini_p->at(2) = 1; 

        // for weistrass initial guess is VERY VERY IMPORTANT -------
        if(weistrass_controlled(ini_p, fun, ptr, root_precision, max_root_find_iterations))
            if(print_debug_data)
                std::cout<<"\nweistrass suceeded\n";

        }
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
        
        if(print_debug_data)
            std::cout<<"\nans : "<<z<<"\t"<<zl;
        
    }
    if(print_z)
        std::cout<<"\nCompressibility factors are : "<<z<<"\t"<<zl<<"\n";
}
// ------------------------------- dew pt calc --------------------
void pr_eos::calc_phi(float Z, std::unique_ptr<std::vector<float>>& phi)
{
    // totally under construction
    // printf("\n vera maari vera maari \n");
    float temp1 = 0, temp2 = 0;
    for(uint16_t i=0;i<size_of_gas_data;i++)
    {
        temp1 = pr_mix_data[i].b / b;
        temp2 = Z - bb;
        if(Z!=0 && temp2>0)
            phi->at(i) = exp(((temp1) * (Z - 1)) - 
                            (log(temp2)) -
                            (0.35355339059327373 * aa / bb) * 
                            ((2 * aik[i] / a) - temp1) *
                            log((Z + 2.414213562373095*bb) / (Z - 0.41421356237309515*bb)));

    }
}

float pr_eos::dew_pt_calc(bool is_first_time)
{

    // calculate Phi_Y since anyway this doesnot going to change
    std::fill(phi_V_ptr->begin(), phi_V_ptr->end(), 1);
    calc_phi(z, phi_V_ptr);

    // report Phi_Y values
    if(print_debug_data){
        printf("\n \n Phi vapour values \n\n");
        for(const auto i : *phi_V_ptr)
            std::cout<<i<<"\n";
        printf("\n\n ----------------------------------- \n\n");
    }
    
    uint16_t j = 0;
    float xi_total = 0;
    for (const auto& i : *base_data_pt){
        xi_not_norm[j] = i.yi / exp(log((i.pc * 0.000001) / p) + (5.37269855031944 * (1 + i.w) * (1 - (i.tc / t))));
        xi_total = xi_total + xi_not_norm[j];
        j++;
    }

    // estimated temperature and return / stop the func here itself
    j=0;
    if(is_first_time){
        estimated_temp = 0;
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            i->xi = (xi_not_norm[j] / xi_total);
            estimated_temp = estimated_temp + i->tsat * i->xi;
            j++;
        }
        return 0;
    }

    // find xi_initial
    for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
        i->xi = (xi_not_norm[j] / xi_total);
        j++;
    }


    // find the phi l
    std::fill(phi_L_ptr->begin(), phi_L_ptr->end(), 1);

    float xi_total_new = 0;
    xi_total = 0;
    // loop to refine xi
    for (uint32_t i = 0; i < 20; i++){
        j=0;
        should_I_use_yi = false;
        // pr_mix_data.clear();
        zl=0;
        getZ(false);
        if(zl<=0)
            break;

        calc_phi(zl,phi_L_ptr);

        if(print_debug_data){
            //print result
            j = 0;
            printf("\nvalues at %dth iteration \n", i);
            std::cout << std::left << std::setw(20) << "Phi_L" << std::left << std::setw(20) << "Xi_norm" << std::left << std::setw(20) << "Xi" << std::endl;
            for(const auto& i : *base_data_pt){
                std::cout << std::left << std::setw(20) << phi_L_ptr->at(j) << std::left << std::setw(20) << i.xi << std::left << std::setw(20) << xi_not_norm[j] << std::endl;
                j++;
            }
        }

        // update xi using the yi, phi_v and phi_l and 
        j=0; xi_total_new=0;
        for(auto& i : *base_data_pt){
            xi_not_norm[j] = i.yi * (phi_V_ptr->at(j) / phi_L_ptr->at(j));
            xi_total_new = xi_total_new + xi_not_norm[j];
            j++;
        }

        // loop to normalize and write values to base_Data -> Xi
        j = 0;
        for(auto i = base_data_pt->begin(); i != base_data_pt->end(); ++i){
            i->xi = xi_not_norm[j] / xi_total_new;
            j++;
        }


        if(abs(xi_total_new - xi_total) <= xi_total_tolerance)
            break;
        
        xi_total = xi_total_new;

    }

    return xi_total_new - 1;
}

void pr_eos::calc_dew()
{
    float t2 = 0, t1 = 0, fx = 0, tnew = 0;

    getZ(false);
    dew_pt_calc(true);
    printf("\n\n Estimated dewpt temperature is %f Deg C\n",estimated_temp-273.15);

    t1 = estimated_temp - 25.0;
    t2 = estimated_temp + 25.0;
    
    //secant
    float tn = 0, ft, ftt;
    uint16_t i;
    for (i = 0; i < max_root_find_iterations; i++){
        t = t1;
        ft = dew_pt_calc(false);
        t = t2;
        ftt = dew_pt_calc(false);
        float temp = (ft - ftt);
        if (abs(temp) <= root_precision)
            break;

        tn = t1 - ft * ((t1 - t2) / (ft - ftt));

        t1 = t2;
        t2 = tn;
    }
    printf("\n\n Dew pt temperature is (converged at %d steps): %f  Deg C\n\n",i,tn-273.15);
}

void pr_eos::calc_cp()
{
    getZ(true);
    //cp const data
    std::unique_ptr<std::vector<CP_Const>> cp_const = mydbclass.get_cp_const_pointer();
    // CpIG = A + B.T + C.T² + D.T³
    // float* cp_R = new float[size_of_gas_data];
    cp_ig = 0; // gives the Cp in CAL/G-MOLE-K
    uint16_t j = 0;
    for(const auto& i : *base_data_pt)
        cp_ig = cp_ig +  i.yi*(cp_const->at(j).A + t_reserve*cp_const->at(j).B 
                                + t_reserve*t_reserve*cp_const->at(j).C
                                + t_reserve*t_reserve*t_reserve*cp_const->at(j).D);

    cp_ig = 4.1868 * cp_ig; // converted to J / mol . K
    cv_ig = cp_ig - r;
    // printf("\n\ncp : %f   cv : %f\n\n",cp_ig, cv_ig);
    // now the struggle starts
    cp_r = 0; cv_r = 0;
    j=0;
    float v = ((z * r * t) / p);
    float dPdVatT = 0, dadTatV = 0, dPdTatV = 0, dTdPatV = 0, daadTatP= 0, 
          dbbdTatP= 0, aai     = 0, bbi     = 0, Num     = 0, Denom   = 0, dZdTatP = 0, 
          dVdTatP = 0, aDoubleD= 0, Cvr     = 0, Cpr     = 0, aDoubleDash = 0;
    // std::cout<<"\n\n Volume is --> "<<v<<"\n";
    for(const auto& i : pr_mix_data)
    {
        // step 1
        dPdVatT = ((-r * t) / ((v - i.b) * (v - i.b))) + 2 * i.a * (v + i.b) / ((v * (v + i.b) + i.b * (v - i.b)) * (v * (v + i.b) + i.b * (v - i.b)));
        
        //report
        /**
        std::cout<<"PR props :\n"
                 <<"P : "
                 <<p
                 <<"\nt : "
                 <<t
                 <<"\na : "
                 <<i.a
                 <<"\nb : "
                 <<i.b
                 <<"\nm : "
                 <<i.k
                 <<"\n\n";
        **/
        // step 2
        dadTatV = -i.k * i.ac / (sqrt(t * base_data_pt->at(j).tc) * (1 + i.k * (1 - sqrt(t / base_data_pt->at(j).tc))));
        dPdTatV = r / (v - i.b) - dadTatV / (v * (v + i.b) + i.b * (v - i.b));
        dTdPatV = 1 / dPdTatV;

        // step 3 
        daadTatP = (p / (r * r * t_reserve * t_reserve)) * (dadTatV - 2 * i.a / t_reserve);
        dbbdTatP = -i.b * p / (r * t_reserve * t_reserve);

        aai = i.a * p / (r * r * t * t);  // A constant for Z-equation
        bbi = i.b * p / (r * t);          // B constant for Z-equation

        Num = daadTatP * (bbi - z) + dbbdTatP * (6 * bbi * z + 2 * z - 3 * bbi * bbi - 2 * bbi + aai - z * z);
        Denom = 3 * z * z + 2 * (bbi - 1) * z + (aai - 2 * bbi - 3 * bbi * bbi);

        dZdTatP = Num / Denom;

        dVdTatP = (r / p) * (t_reserve * dZdTatP + z);



        // last step to success
        aDoubleDash = i.ac * i.k * (1 + i.k) * sqrt(base_data_pt->at(j).tc / t_reserve) / (2 * t_reserve * base_data_pt->at(j).tc);
        Cvr = ( (t_reserve * aDoubleDash / (i.b * sqrt(8))) * log((z + bbi * (1 + sqrt(2))) / (z + bbi * (1 - sqrt(2)))) );
        Cpr = Cvr + t_reserve * dPdTatV * dVdTatP - r;
        
        // report
        /**
        std::cout<<"dPdVatT : "
                 <<dPdVatT
                 <<"\ndPdTatV : "
                 <<dPdTatV
                 <<"\ndTdPatV : "
                 <<dTdPatV
                 <<"\ndaadTatP: "
                 <<daadTatP
                 <<"\ndbbdTatP : "
                 <<dbbdTatP
                 <<"\ndZdTatP : "
                 <<dZdTatP
                 <<"\ndVdTatP : "
                 <<dVdTatP
                 <<"\naDoubleDash : "
                 <<aDoubleDash
                 <<"\nCvr : "
                 <<Cvr
                 <<"\nCpr : "
                 <<Cpr
                 <<"\n\n\n";
        **/
        // atlast
        cp_r = cp_r + base_data_pt->at(j).yi*Cpr;
        cv_r = cv_r + base_data_pt->at(j).yi*Cvr;
        j++;
    }

    cp = cp_r + cp_ig;
    cv = cv_r + cv_ig;
    // printf("\n\ncpR : %f   cvR : %f\n\n",cp_r, cv_r);

}

void pr_eos::get_cp(bool print_val)
{
    calc_cp();
    if(print_val){
        std::cout<<"\ncp value is "<<cp<<"\n";
        std::cout<<"\ncv value is "<<cv<<"\n";
        std::cout<<"\ncpR value is "<<cp_r<<"\n";
        std::cout<<"\ncvR value is "<<cv_r<<"\n";
        std::cout<<"\ncpIG value is "<<cp_ig<<"\n";
        std::cout<<"\ncvIG value is "<<cv_ig<<"\n";
        std::cout<<"\nlambda value for real gas is "<<cp/cv<<"\n";
        std::cout<<"\nlambda value for ideal gas is "<<cp_ig/cv_ig<<"\n\n";
    }
}