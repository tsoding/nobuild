#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(void)
{
#if _WIN32
    WARN("Piping is not implemented on Windows yet");
#else
    PIPE(IN(PATH("examples", "pipe.c")),
         CHAIN("sort", "-rn"),
         CHAIN("rev"),
         CHAIN("base64"),
         CHAIN("gzip", "-f"),
         CHAIN("hexdump", "-C"));
#endif

    return 0;
}
