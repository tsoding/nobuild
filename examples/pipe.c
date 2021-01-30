#define NOBUILD_IMPLEMENTATION
#include "../nobuild.h"

int main(void)
{
#if _WIN32
    WARN("Piping is not implemented on Windows yet");
#else
    PIPE(IN("./nobuild.c"), CHAIN("cat"));
#endif

    return 0;
}
