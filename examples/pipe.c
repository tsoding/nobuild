#define NOBUILD_IMPLEMENTATION
#include "../nobuild2.h"

int main(void)
{
    CHAIN(IN(PATH("examples", "pipe.c")),
          CHAIN_CMD(PATH("tools", "rot13")),
          CHAIN_CMD(PATH("tools", "hex")),
          OUT("output.txt"));
    INFO("------------------------------------------------------------");
    CMD(PATH("tools", "cat"), "output.txt");
    INFO("------------------------------------------------------------");

#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif

    return 0;
}
