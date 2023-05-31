#include<vector>
#include<memory>
// float newton_raphson(float* initial_value, 
//                      float (*func)(float, std::shared_ptr<std::vector<float>>), 
//                      float (*funcd)(float, std::shared_ptr<std::vector<float>>), 
//                      std::shared_ptr<std::vector<float>> parameters, 
//                      float tolerance)
// {
//     float Next_value = *initial_value - (func(*initial_value, parameters) / funcd(*initial_value, parameters));
//     //report("Next value",Next_value);

//     // to find the diff
//     float diff = Next_value - *initial_value;

//     // to null the minus sign --> finds abs of diff
//     // if (diff < 0) diff = diff * -1;
//     *(int *)&diff = *(int *)&diff & 0b01111111111111111111111111111111;
//     //std::cout<<count;  
//     //report("diff",diff);

//     if(diff >= tolerance)
//     {
//         newton_raphson(&Next_value, (*func), (*funcd), parameters, tolerance);
//     }
//     else
//         return Next_value;
// }

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



                           
bool weistrass_controlled(std::shared_ptr<std::vector<float>> initial_val, 
                          float (*func)(float, std::shared_ptr<std::vector<float>>), 
                          std::shared_ptr<std::vector<float>> parameters, 
                          float tolerance,
                          uint16_t controlled_iter)
{
    float temp = 1;
    uint16_t success = 0;
    uint16_t degree = (*initial_val).size();
    float* z_new = new float[degree];
    float* error = new float[degree];

    for(unsigned int i=0;i<degree;i++)
        z_new[i]=0.0;
    
// for loop implementation
    uint16_t i = 0;
    for(i=0;i<controlled_iter;i++)
    {
        success = 0;
        for(uint16_t j=0;j<degree;j++)
        {
            temp = 1;
            for(uint16_t z=0;z<degree;z++)
                if(z!=j)
                    temp = temp * ((*initial_val)[j]-(*initial_val)[z]);
                    
            z_new[j] = (*initial_val)[j] - (func((*initial_val)[j],parameters) / temp);
            error[j] = (*initial_val)[j] - z_new[j];

            // bit hack - calculates absolute
            *(int *)&error[j] = *(int *)&error[j] & 0b01111111111111111111111111111111;

            (*initial_val)[j] = z_new[j];
        }

        for(unsigned int k=0;k<degree;k++)
            if(error[k]<=tolerance)
                success++;

        if(success==degree)
        {
            delete[] z_new;
            delete[] error;
            for(unsigned int k=0;k<degree;k++)
                (*initial_val)[k] = z_new[k];
            return true;
        }

    }

    delete[] z_new;
    delete[] error;
    return false;
}