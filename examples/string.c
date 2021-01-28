#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define DEMO(expr)                              \
    INFO(#expr " == \"%s\"", expr)

int main(int argc, char *argv[])
{
    DEMO(CONCAT("foo", "bar", "baz"));
    DEMO(PATH("foo", "bar", "baz"));
    DEMO(JOIN("++", "foo", "bar", "baz"));
    DEMO(NOEXT("main.c"));
    return 0;
}
