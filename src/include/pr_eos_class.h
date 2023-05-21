#include <vector>
#include<memory>


// //gas base properties definition
// struct base_props
// {
//     float tc, pc, w;
// };

class pr_eos
{
private:
// private variable
    std::unique_ptr<std::vector<base_props>> base_data;
// private member funcs
    void pr_mix_report(PR_props* pr);

public:
// public variables

// public member funcs
    pr_eos(float pressure, float temperature, const char* db_name);
    void print_base();
    ~pr_eos();
    int omale();

};