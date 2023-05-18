float newton_raphson(float* initial_value, 
                     float (*func)(float, std::shared_ptr<std::vector<float>>), 
                     float (*funcd)(float, std::shared_ptr<std::vector<float>>), 
                     std::shared_ptr<std::vector<float>> parameters, 
                     float tolerance);

bool newton_raphson_controlled(float* initial_value, 
                     float (*func)(float, std::shared_ptr<std::vector<float>>), 
                     float (*funcd)(float, std::shared_ptr<std::vector<float>>), 
                     std::shared_ptr<std::vector<float>> parameters, 
                     float tolerance,
                     uint16_t controlled_iter);

bool weistrass_controlled(std::shared_ptr<std::vector<float>> initial_val, 
                          float (*func)(float, std::shared_ptr<std::vector<float>>), 
                          std::shared_ptr<std::vector<float>> parameters, 
                          float tolerance,
                          uint16_t controlled_iter);