#include <string>
#include <vector>
#include <set>
#include <map>
#include<memory>
#include<iostream>

// data structs
struct CP_Const
{
    float A, B, C, D;
};

//PR EOS properties definition
struct PR_props
{
    float a, b, aa, bb, k, alpha, ac, c, d, e;
};

//gas base properties definition
struct base_props
{
    float tc, pc, w;
};

class pr_eos
{
private:
// private variable
    std::unique_ptr<std::vector<base_props>> base_data;
// private member funcs
    void pr_mix_report(PR_props* pr);

public:
// public variables
    pr_eos(std::unique_ptr<std::vector<base_props>> passed_data_ptr);
    void print_base();
// public member funcs
};

pr_eos::pr_eos(std::unique_ptr<std::vector<base_props>> passed_data_ptr)
{
    base_data = std::move(passed_data_ptr);
}
void pr_eos::print_base()
{
    if(base_data!=nullptr)
        for(auto i = base_data->begin(); i != base_data->end(); ++i) 
            std::cout<<"\n"<<i->tc<<"\t"<<i->pc<<"\t"<<i->w<<"\n";
}

void pr_mix_report(PR_props* pr)
{
    std::cout << "\n m = " << pr->k;
    std::cout << "\n ac = " << pr->ac;
    std::cout << "\n alpha = " << pr->alpha;
    std::cout << "\n a = " << pr->a;
    std::cout << "\n b = " << pr->b;
    std::cout << "\n A = " << pr->aa;
    std::cout << "\n B = " << pr->bb;
    std::cout << "\n C = " << pr->c;
    std::cout << "\n D = " << pr->d;
    std::cout << "\n E = " << pr->e << "\n";
}