#define NOBUILD_IMPLEMENTATION
#include "./nobuild.h"

#define CFLAGS "-Wall", "-Wextra", "-std=c11", "-pedantic", "-ggdb"

void check_example(const char *example)
{
    const char *example_path = PATH("examples", NOEXT(example));

    INFO("===== %s =====", NOEXT(example));

#ifdef _WIN32
    CMD("cl.exe", "/Fe.\\examples\\", PATH("examples", example));
    CMD(CONCAT(example_path, ".exe"));
#else
    const char *cc = getenv("CC");
    if (cc == NULL) cc = "cc";

    CMD(cc, CFLAGS, "-o", example_path, PATH("examples", example));
    CMD(example_path);
#endif // _WIN32
}

int main(int argc, char *argv[])
{
    FOREACH_FILE_IN_DIR(example, "examples", {
        if (ENDS_WITH(example, ".c")) {
            check_example(example);
        }
    });
    return 0;
}
