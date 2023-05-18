#include<iostream>
#include<vector>
#include<memory>
#include "nr.h"
float func(float x, std::shared_ptr<std::vector<float>> parameters)
{
    float temp = 2*x*x*x - 2*x - (*parameters)[0];
    return temp;
}

float func_derv(float x, std::shared_ptr<std::vector<float>> parameters)
{
    float temp = 6*x*x - 2;
    return temp;
}

float func3(float x, std::shared_ptr<std::vector<float>> parameters)
{
    float temp = x*x*x - (*parameters)[0]*x*x + (*parameters)[1]*x - (*parameters)[2];
    return temp;
}

int main()
{
    std::vector<float> parameters;
    parameters.push_back(5);

    std::shared_ptr<std::vector<float>> ptr;

    ptr = std::make_shared<std::vector<float>>(parameters);

    float (*fun)(float, std::shared_ptr<std::vector<float>>) = func;
    float (*funcd)(float, std::shared_ptr<std::vector<float>>) = func_derv;

    float f = 1.0;

    float ans = newton_raphson(&f,fun,funcd,ptr,0.0001);

    std::cout<<"ans : "<<fun(0,ptr);
    std::cout<<"\nans : "<<ans;
    f = 1.0;
    if(newton_raphson_controlled(&f,fun,funcd,ptr,0.0001,50))
        std::cout<<"\nans from controlled : "<<f;

// ---------------------------------------------------------------------
    std::vector<float> parameters_w;
    std::vector<float> initial;
    parameters_w.push_back(6);
    parameters_w.push_back(11);
    parameters_w.push_back(6);

    initial.push_back(1);
    initial.push_back(1);
    initial.push_back(1);

    std::shared_ptr<std::vector<float>> ptr_w;
    std::shared_ptr<std::vector<float>> ini_p;

    ptr_w = std::make_shared<std::vector<float>>(parameters_w);
    ini_p = std::make_shared<std::vector<float>>(initial);

    float (*fun3)(float, std::shared_ptr<std::vector<float>>) = func3;

    std::cout<<"\nweistrass func = "<<fun3(0, ptr_w);

    if(weistrass_controlled(ini_p, fun3, ptr_w, 0.01, 50))
        std::cout<<"\nsuceeded\n";
    
    for(const auto& i : *ini_p)
        std::cout<<i<<"\n";

    std::cout<<"\n\nmain";
    return 0;
}