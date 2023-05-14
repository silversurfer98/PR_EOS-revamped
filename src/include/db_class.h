#include <vector>
#include<memory>

struct CP_Const
{
    float A, B, C, D;
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
    ~db_class();

};