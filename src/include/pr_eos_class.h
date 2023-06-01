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
    float z, zl;
    float p, t;

// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void getZ(bool Calc_phi);

};