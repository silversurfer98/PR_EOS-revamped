#include<vector>
#include<memory>
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



bool newton_raphson_controlled(float* initial_value, 
                     float (*func)(float, std::shared_ptr<std::vector<float>>), 
                     float (*funcd)(float, std::shared_ptr<std::vector<float>>), 
                     std::shared_ptr<std::vector<float>> parameters, 
                     float tolerance,
                     uint16_t controlled_iter)
{
    uint16_t i = 0;
    for(i=0;i<=controlled_iter;i++)
    {
        float Next_value = *initial_value - (func(*initial_value, parameters) / funcd(*initial_value, parameters));

        // to find the diff
        float diff = Next_value - *initial_value;

        // to null the minus sign --> finds abs of diff
        // if (diff < 0) diff = diff * -1; bit hack
        *(int *)&diff = *(int *)&diff & 0b01111111111111111111111111111111;

        if(diff >= tolerance)
            *initial_value = Next_value;
        else
            return true;
    }
    return false;
}