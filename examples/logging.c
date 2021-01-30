#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(void)
{
    INFO("    Informational Message");
    WARN("    Warning Message");
    ERRO("    Error Message");

    return 0;
}
