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
    float cp, cv;

    // db_class mydbclass;

// public member funcs
    pr_eos(float pressure_in_BarA, float temperature_in_C, const char* db_name, bool calc_dewpt);
    pr_eos(float pressure_in_BarA, float temperature_in_C, bool calc_dewpt);
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