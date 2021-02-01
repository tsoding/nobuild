#define NOBUILD_IMPLEMENTATION
#include "./nobuild2.h"

#define CFLAGS "-Wall", "-Wextra", "-std=c11", "-pedantic"

void build_tools(void)
{
    FOREACH_FILE_IN_DIR(tool, "tools", {
        if (ENDS_WITH(tool, ".c")) {
            Cstr tool_path = PATH("tools", tool);
#ifndef _WIN32
            CMD("cc", CFLAGS, "-o", NOEXT(tool_path), tool_path);
#else
            CMD("cl.exe", "/Fe.\\tools\\", tool_path);
#endif
        }
    });
}

void run_examples(void)
{
    FOREACH_FILE_IN_DIR(example, "examples", {
        if (ENDS_WITH(example, ".c")) {
            Cstr example_path = PATH("examples", example);
#ifndef _WIN32
            CMD("cc", CFLAGS, "-o", NOEXT(example_path), example_path);
#else
            CMD("cl.exe", "/Fe.\\examples\\", example_path);
#endif
            CMD(NOEXT(example_path));
        }
    });
}

int main(void)
{
    build_tools();
    run_examples();

    return 0;
}
