#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define DEMO_S(expr)                         \
    INFO("    " #expr " == \"%s\"", expr)

#define DEMO_D(expr)                         \
    INFO("    " #expr " == %d", expr)

int main(void)
{
    DEMO_S(CONCAT("foo", "bar", "baz"));
    DEMO_S(PATH("foo", "bar", "baz"));
    DEMO_S(JOIN("++", "foo", "bar", "baz"));
    DEMO_S(NOEXT("main.c"));
    DEMO_D(ENDS_WITH("main.c", ".c"));
    DEMO_D(ENDS_WITH("main.java", ".c"));
    DEMO_D(ENDS_WITH("", ".c"));
    return 0;
}
