#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

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

int main(void)
{
    DEMO(foreach_file_in_dir("."));

    return 0;
}
