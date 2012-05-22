#include <iostream>
#include "cppbin/BinTest.h"

#include "cppstaticlib/StaticLibTest.h"
#include "cppdynamiclib/DynamicLibTest.h"

int main(int argc, char **argv)
{
    std::cout << "TestStaticDouble(3): " << CStaticCalc::static_double_me(3) << std::endl;
    std::cout << "TestDynamicDouble(5): " << CDynamicCalc::dynamic_double_me(5) << std::endl;
    return 0;
}

