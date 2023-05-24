#include <vector>
#include<memory>

class pr_eos
{
public:
// public variables

// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    ~pr_eos();
    void print_base_data();
    void print_bip_data();
    void construct_pr_props();

};