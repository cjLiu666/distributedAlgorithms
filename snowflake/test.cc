#include "Snowflake.hpp"
#include "Snowflake.cc"
#include <iostream>
using namespace distributedAlgo;

int main() {
    Snowflake<1534832906275L> uuid(1, 1);
    for (size_t i = 0; i < 100; i++) {
        std::cout << uuid.nextid() << std::endl;
    }
    return 0;
}