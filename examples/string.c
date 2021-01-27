#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define DEMO(expr)                              \
    printf(#expr " == \"%s\"\n", expr)

int main(int argc, char *argv[])
{
    printf("=== String Manipulation Example ===\n");
    DEMO(CONCAT("foo", "bar", "baz"));
    DEMO(PATH("foo", "bar", "baz"));
    DEMO(CONCAT_SEP("++", "foo", "bar", "baz"));
    DEMO(remove_ext("main.c"));
    return 0;
}
