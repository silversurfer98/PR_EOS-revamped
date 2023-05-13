#include "db_class.h"
#include <iostream>
int main()
{
    db_class myclass("props.db");
    unsigned int res = myclass.get_all_gas_names();
    if(res==0)
        myclass.prepare_bip();
    return 0;
}