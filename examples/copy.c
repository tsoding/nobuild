#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define CFLAGS "-Wall", "-Wextra", "-std=c99", "-pedantic"

#define DEMO(expr)   \
    do {             \
        INFO(#expr); \
        expr;        \
    } while(0)

void copy(const char *from, const char *to)
{
    COPY(from, to);
}

void copy_each_file_in_dir(const char *from, const char *to)
{
    COPY_EACH_FILE_IN_DIR(from, to);
}

int main(int argc, char **argv)
{
    MKDIRS(PATH("examples", "copy-test"));

    DEMO(copy(PATH("examples", "copy.c"), PATH("examples", "copy-test", "copy.c")));

    DEMO(copy_each_file_in_dir("examples", PATH("examples", "copy-test")));

    RM (PATH("examples", "copy-test"));

    return 0;
}