#include <vector>
#include<memory>
class db_class
{
public:
// variables

//class members
    db_class(const char* custom_filename);
    unsigned int get_all_gas_names();
    std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();
    unsigned int cp_const_data_aquisition();


};

struct CP_Const
{
    float A, B, C, D;
};