#include <vector>
#include<memory>

//PR EOS properties definition
// struct PR_props
// {
//     float a, b, k, alpha, ac;
// };


class pr_eos
{
public:
// public variables
    std::unique_ptr<std::vector<base_props>> base_data_pt;
    float z, zl;
    float p, t;
    bool use_trig_method;
    float root_precision;
    uint16_t max_root_find_iterations;
    float xi_total_tolerance;
    bool print_debug_data;
    db_class mydbclass;

// public member funcs
    pr_eos(float pressure_in_BarA, float temperature_in_C, const char* db_name, bool calc_dewpt);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void getZ();
    void calc_dew();
    
};