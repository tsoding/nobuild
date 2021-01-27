#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

void foreach_vargs(int ignore, ...)
{
    va_list args;

    FOREACH_VARGS(ignore, arg, args, {
        printf("    %s\n", arg);
    });
}

void foreach_array(void)
{
    static const char *array[] = {
        "foo", "bar", "baz"
    };

    FOREACH_ARRAY(const char *, item, array, {
        printf("    %s\n", item);
    });
}

void foreach_file_in_dir(const char *dir_path)
{
    FOREACH_FILE_IN_DIR(file, dir_path, {
            printf("    %s\n", file);
    });
}

#define DEMO(expr)                              \
    do {                                        \
        printf(#expr"\n");                      \
        expr;                                   \
    } while(0)

int main(int argc, char *argv[])
{
    printf("=== Foreach Macro Example ===\n");
    DEMO(foreach_vargs(69, "foo", "bar", "baz", NULL));
    DEMO(foreach_array());
    DEMO(foreach_file_in_dir("."));

    return 0;
}
