#include <string>
#include <vector>
#include<memory>
class db_class
{
public:
// variables

//class members
    db_class(const char* custom_filename);
    unsigned int get_all_gas_names();
    void choosen_gas_querry();
    unsigned int prepare_bip();
    std::unique_ptr<std::vector<std::vector<float>>> get_bip_pointer();

};