#define NOBUILD_IMPLEMENTATION
#include "../nobuild2.h"

int main(void)
{
#if _WIN32
    WARN("Piping is not implemented on Windows yet");
#else
    CHAIN(IN(PATH("examples", "pipe.c")),
          CHAIN_CMD(PATH("tools", "rot13")),
          CHAIN_CMD(PATH("tools", "hex")));
#endif

    return 0;
}
