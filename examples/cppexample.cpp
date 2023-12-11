#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#include <iostream>

int main(int argc, char **argv)
{
    GO_REBUILD_URSELF(argc, argv);

    std::cout << "Hello, world!" << std::endl;

    return 0;
}
