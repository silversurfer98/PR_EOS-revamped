#include<vector>
#include<memory>
float Next_value = 0;
float newton_raphson(float* initial_value, 
                     float (*func)(float, std::shared_ptr<std::vector<float>>), 
                     float (*funcd)(float, std::shared_ptr<std::vector<float>>), 
                     std::shared_ptr<std::vector<float>> parameters, 
                     float tolerance)
{
    float Next_value = *initial_value - (func(*initial_value, parameters) / funcd(*initial_value, parameters));
    //report("Next value",Next_value);

    // to find the diff
    float diff = Next_value - *initial_value;

    // to null the minus sign --> finds abs of diff
    // if (diff < 0) diff = diff * -1;
    *(int *)&diff = *(int *)&diff & 0b01111111111111111111111111111111;
    //std::cout<<count;  
    //report("diff",diff);

    if(diff >= tolerance)
    {
        newton_raphson(&Next_value, (*func), (*funcd), parameters, tolerance);
    }
    else
        return Next_value;
}