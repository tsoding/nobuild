#define NOBUILD_IMPLEMENTATION
#include "./nobuild2.h"

#define CFLAGS "-Wall", "-Wextra", "-std=c11", "-pedantic"

void build_tool(const char *tool)
{
    Cstr tool_path = PATH("tools", tool);
#ifndef _WIN32
    CMD("cc", CFLAGS, "-o", NOEXT(tool_path), tool_path);
#else
    CMD("cl.exe", "/Fe.\\tools\\", tool_path);
#endif
}

void build_tools(void)
{
    FOREACH_FILE_IN_DIR(tool, "tools", {
        if (ENDS_WITH(tool, ".c")) {
            build_tool(tool);
        }
    });
}

void run_example(const char *example)
{
    Cstr example_path = PATH("examples", example);
#ifndef _WIN32
    CMD("cc", CFLAGS, "-o", NOEXT(example_path), example_path);
#else
    CMD("cl.exe", "/Fe.\\examples\\", example_path);
#endif
    CMD(NOEXT(example_path));
}

void run_examples(void)
{
    FOREACH_FILE_IN_DIR(example, "examples", {
        if (ENDS_WITH(example, ".c")) {
            run_example(example);
        }
    });
}

int main(void)
{
    INFO("test");
    build_tool("rot13.c");
    // build_tools();
    // run_examples();

    return 0;
}
