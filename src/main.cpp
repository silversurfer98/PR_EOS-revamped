#include "db_class.h"
#include <iostream>
int main()
{
    db_class myclass("props.db");
    unsigned int res = myclass.print_all_gas();
    std::cout<<"\n\n res = "<<res;
    return 0;
}