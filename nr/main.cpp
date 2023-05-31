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

float funcZ(float value, std::shared_ptr<std::vector<float>> parameters)
{
    // f(Z) = Z3 - CZ2 + DZ - E = 0
    return value*value*value - (*parameters)[0] * value*value + (*parameters)[1] * value - (*parameters)[2];
}

int main()
{
/***
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
***/
// ---------------------------------------------------------------------
    std::vector<float> parameters_w;
    std::vector<float> initial;
    parameters_w.push_back(0.997698);
    parameters_w.push_back(0.0156476);
    parameters_w.push_back(4.134e-05);

    initial.push_back(0);
    initial.push_back(0.5);
    initial.push_back(1);

    std::shared_ptr<std::vector<float>> ptr_w;
    std::shared_ptr<std::vector<float>> ini_p;

    ptr_w = std::make_shared<std::vector<float>>(parameters_w);
    ini_p = std::make_shared<std::vector<float>>(initial);

    float (*fun)(float, std::shared_ptr<std::vector<float>>) = funcZ;

    std::cout<<"\nweistrass func = "<<fun(1, ptr_w)<<"\n\n";

    if(weistrass_controlled(ini_p, fun, ptr_w, 0.0001, 50))
        std::cout<<"\nsuceeded\n";
    
    for(const auto& i : initial)
        std::cout<<i<<"\n";

    std::cout<<"\n\nend of nr main";
    return 0;
}