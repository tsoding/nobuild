#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

#define DEMO(expr)                              \
    INFO("    "#expr" == %d", expr)

void print_file_recursively(const char *path)
{
    FOREACH_FILE_IN_DIR(file, path, {
        const char *child = PATH(path, file);
        INFO("    %s", child);
        if (*file != '.' && IS_DIR(child)) {
            print_file_recursively(child);
        }
    });
}

int main(int argc, char *argv[])
{
    DEMO(IS_DIR("./nobuild.c"));
    DEMO(IS_DIR("./examples"));
    DEMO(IS_DIR("./file_that_does_not_exist"));

    INFO("Recursively traversing the file system");
    print_file_recursively(".");

    INFO("Directory removal");
    MKDIRS("foo", "bar", "baz");
    MKDIRS("foo", "bar", "hello", "world");
    RM("foo");
    DEMO(IS_DIR("foo"));

    return 0;
}
