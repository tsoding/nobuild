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
    print_cstr_array(cstr_array_make(NULL));
    print_cstr_array(cstr_array_make("foo", NULL));
    print_cstr_array(cstr_array_make("foo", "bar", NULL));
    print_cstr_array(cstr_array_make("foo", "bar", "baz", NULL));
    return 0;
}
