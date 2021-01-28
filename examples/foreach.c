#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

void foreach_vargs(int ignore, ...)
{
    va_list args;

    FOREACH_VARGS(ignore, arg, args, {
        INFO("    %s", arg);
    });
}

void foreach_array(void)
{
    static const char *array[] = {
        "foo", "bar", "baz"
    };

    FOREACH_ARRAY(const char *, item, array, {
        INFO("    %s", item);
    });
}

void foreach_file_in_dir(const char *dir_path)
{
    FOREACH_FILE_IN_DIR(file, dir_path, {
            INFO("    %s", file);
    });
}

#define DEMO(expr)                              \
    do {                                        \
        INFO(#expr);                            \
        expr;                                   \
    } while(0)

int main(int argc, char *argv[])
{
    DEMO(foreach_vargs(69, "foo", "bar", "baz", NULL));
    DEMO(foreach_array());
    DEMO(foreach_file_in_dir("."));

    return 0;
}
