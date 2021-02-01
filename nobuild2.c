#define NOBUILD_IMPLEMENTATION
#include "./nobuild2.h"

void print_cstr_array(Cstr_Array cstrs)
{
    printf("#");
    for (size_t i = 0; i < cstrs.count; ++i) {
        printf("%s ", cstrs.elems[i]);
    }
    printf("#\n");
}

int main(void)
{
    printf("%s\n", PATH("foo", "bar"));
    printf("%s\n", CONCAT("foo", "bar"));
    printf("%s\n", JOIN("++", "foo", "bar", "baz"));
    return 0;
}
