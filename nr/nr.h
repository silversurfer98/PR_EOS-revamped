float newton_raphson(float* initial_value, 
                     float (*func)(float, std::shared_ptr<std::vector<float>>), 
                     float (*funcd)(float, std::shared_ptr<std::vector<float>>), 
                     std::shared_ptr<std::vector<float>> parameters, 
                     float tolerance);