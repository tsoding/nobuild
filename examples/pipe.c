#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(void)
{
#if _WIN32
    WARN("Piping is not implemented on Windows yet");
#else
    Cmd chain[] = {
        // {.args = nobuild_cstr_vargs_to_array(69, "seq", "100", "110", NULL)},
        {.args = nobuild_cstr_vargs_to_array(69, "sort", "-rn", NULL)},
        {.args = nobuild_cstr_vargs_to_array(69, "rev", NULL)},
        {.args = nobuild_cstr_vargs_to_array(69, "base64", NULL)},
        {.args = nobuild_cstr_vargs_to_array(69, "gzip", "-f", NULL)},
        {.args = nobuild_cstr_vargs_to_array(69, "hexdump", "-C", NULL)},
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
#endif

    return 0;
}
