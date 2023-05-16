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
    pr_eos(std::unique_ptr<std::vector<base_props>>);
    void print_base();
// public member funcs
};