#define NOBUILD_IMPLEMENTATION
#include "../nobuild2.h"

#define DEMO(expr)                              \
    INFO("    "#expr" == %d", expr)

void print_file_recursively(const char *path)
{
    INFO("    %s", path);

    if (IS_DIR(path)) {
        FOREACH_FILE_IN_DIR(file, path, {
            const char *child = PATH(path, file);
            if (*file != '.') {
                print_file_recursively(child);
            }
        });
    }
}

int main(void)
{
    DEMO(IS_DIR("./nobuild.c"));
    DEMO(IS_DIR("./examples"));
    DEMO(IS_DIR("./file_that_does_not_exist"));

    INFO("Recursively traversing the file system");
    print_file_recursively(".");

    INFO("Directory removal");
    MKDIRS("foo", "bar", "baz");
    MKDIRS("foo", "bar", "hello", "world");
    print_file_recursively("foo");
    RM("foo");
    DEMO(IS_DIR("foo"));

    return 0;
}
