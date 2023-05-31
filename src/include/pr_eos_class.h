#include <vector>
#include<memory>

//PR EOS properties definition
struct PR_props
{
    float a, b, k, alpha, ac;
};


class pr_eos
{
public:
// public variables
    std::vector<float> parameters;
    float z, zl;
    
// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void getZ();

};