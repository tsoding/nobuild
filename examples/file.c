#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define DEMO(expr)                              \
    INFO(#expr" == %d", expr)

int main(int argc, char *argv[])
{
    DEMO(IS_DIR("./nobuild.c"));
    DEMO(IS_DIR("./examples"));
    DEMO(IS_DIR("./file_that_does_not_exist"));
    return 0;
}
