#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(void)
{
#if _WIN32
    WARN("Piping is not implemented on Windows yet");
#else
    Cmd chain[] = {
        // {.args = nobuild_cstr_vargs_to_array(69, "seq", "100", "110", NULL)},
        {.args = nobuild_cstr_vargs_to_array(NULL, "sort", "-rn", NULL)},
        {.args = nobuild_cstr_vargs_to_array(NULL, "rev", NULL)},
        {.args = nobuild_cstr_vargs_to_array(NULL, "base64", NULL)},
        {.args = nobuild_cstr_vargs_to_array(NULL, "gzip", "-f", NULL)},
        {.args = nobuild_cstr_vargs_to_array(NULL, "hexdump", "-C", NULL)},
    };

    Pipe my_pipe = {
        .chain = chain,
        .chain_size = sizeof(chain) / sizeof(chain[0]),
        .input_filepath = "./examples/pipe.c",
        .output_filepath = "output.txt"
    };

    pid_t *cpids = nobuild_spawn_pipe(&my_pipe);

    for (size_t i = 0; i < my_pipe.chain_size; ++i) {
        nobuild__posix_wait_for_pid(cpids[i]);
    }

    // PIPE(IN(PATH("examples", "pipe.c")),
    //      CHAIN("sort", "-rn"),
    //      CHAIN("rev"),
    //      CHAIN("base64"),
    //      CHAIN("gzip", "-f"),
    //      CHAIN("hexdump", "-C"),
    //      OUT("output.txt"));
#endif

    return 0;
}
