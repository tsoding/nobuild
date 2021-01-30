#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(void)
{
    PIPE(IN("./nobuild.c"), CHAIN("cat"));

    return 0;
}
