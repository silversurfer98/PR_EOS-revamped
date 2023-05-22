#include <vector>
#include<memory>

struct CP_Const
{
    float A, B, C, D;
};

//PR EOS properties definition
struct PR_props
{
    float a, b, aa, bb, k, alpha, ac, c, d, e;
};

struct base_props
{
    float tc, pc, w;
};

class db_class
{
public:
// variables

//class members
    db_class(const char* custom_filename);
    unsigned int get_all_gas_names();
    unsigned int choose_gas_from_user();
    std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();
    std::unique_ptr<std::vector<CP_Const>> get_cp_const_pointer();
    std::unique_ptr<std::vector<base_props>> get_base_gas_props_ptr();
    void new_base_gas_props_ptr(std::unique_ptr<std::vector<base_props>>& ptr);
    // std::shared_ptr<std::vector<base_props>> get_base_gas_props_ptr();
    unsigned int new_get_base_gas_props(std::unique_ptr<std::vector<base_props>>& base_gas_props_pt);
    virtual ~db_class();


};



// class db_access
// {
//     // if member func return 2 --> db is not open / constructor prob
//     // if return 1 --> its the sqlite command fail
// public:
// // Public variables

// // Public class members
//     db_access(const char* custom_filename);
//     unsigned int get_all_gas_names();
//     unsigned int choose_gas_from_user();
//     ~db_access();
// };

// class bip_matrix : private db_access
// {
// public:
//     bip_matrix(const char* filename) : db_access(filename){}
//     std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();

// };