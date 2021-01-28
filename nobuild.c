#define NOBUILD_IMPLEMENTATION
#include "./nobuild.h"

void check_example(const char *example)
{
    const char *example_path = PATH("examples", NOEXT(example));

    INFO("===== %s =====", NOEXT(example));

#ifdef _WIN32
    CMD("cl.exe", "/Fe.\\examples\\", PATH("examples", example));
    CMD(CONCAT(example_path, ".exe"));
#else
    CMD("cc", "-o", example_path, PATH("examples", example));
    CMD(example_path);
#endif // _WIN32
}

int main(int argc, char *argv[])
{
    FOREACH_FILE_IN_DIR(example, "examples", {
        if(*example != '.') {
            size_t n = strlen(example);
            if (n >= 2 && strcmp(example + n - 2, ".c") == 0) {
                check_example(example);
            }
        }
    });
    return 0;
}
