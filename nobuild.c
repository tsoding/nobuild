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

void check_examples(void)
{
    FOREACH_FILE_IN_DIR(example, "examples", {
        if (ENDS_WITH(example, ".c")) {
            check_example(example);
        }
    });
}

void build_tool(const char *tool)
{
#ifdef _WIN32
    CMD("cl.exe", "/Fe.\\tools\\", PATH("tools", CONCAT(tool, ".c")));
#else
    const char *cc = getenv("CC");
    if (cc == NULL) cc = "cc";
    CMD(cc, "-o", PATH("tools", tool), PATH("tools", CONCAT(tool, ".c")));
#endif // _WIN32
}

void build_tools(void)
{
    FOREACH_FILE_IN_DIR(tool, "tools", {
        if (ENDS_WITH(tool, ".c")) {
            build_tool(NOEXT(tool));
        }
    });
}

int main(int argc, char *argv[])
{
    build_tools();
    check_examples();
    return 0;
}
