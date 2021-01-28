#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(int argc, char *argv[])
{
    INFO("Informational Message");
    WARN("Warning Message");
    ERRO("Error Message");

    return 0;
}
