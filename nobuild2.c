#define NOBUILD_IMPLEMENTATION
#include "./nobuild2.h"

void build_tools(void)
{
    FOREACH_FILE_IN_DIR(tool, "tools", {
        if (ENDS_WITH(tool, ".c")) {
            Cstr tool_path = PATH("tools", tool);
            CMD("cc", "-o", NOEXT(tool_path), tool_path);
        }
    });
}

void run_examples(void)
{
    FOREACH_FILE_IN_DIR(example, "examples", {
        if (ENDS_WITH(example, ".c")) {
            Cstr example_path = PATH("examples", example);
            CMD("cc", "-o", NOEXT(example_path), example_path);
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
