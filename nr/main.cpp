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
    std::cout<<"\n\nmain";
    return 0;
}