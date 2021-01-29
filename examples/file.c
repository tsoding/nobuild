#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define DEMO(expr)                              \
    INFO(#expr" == %d", expr)

int main(int argc, char *argv[])
{
    DEMO(IS_DIR("./nobuild.c"));
    DEMO(IS_DIR("./examples"));
    return 0;
}
