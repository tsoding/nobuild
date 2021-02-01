#include <stdio.h>
#include <ctype.h>

char rot13(char x)
{
    if ('a' <= x && x <= 'z') return ((x - 'a') + 13) % 26 + 'a';
    if ('A' <= x && x <= 'Z') return ((x - 'A') + 13) % 26 + 'A';
    return x;
}

#define BUFFER_SIZE (640 * 1000)

char buffer[BUFFER_SIZE];

int main(void)
{
    while (!feof(stdin)) {
        size_t n = fread(buffer, sizeof(buffer[0]), BUFFER_SIZE, stdin);
        for (size_t i = 0; i < n; ++i) {
            buffer[i] = rot13(buffer[i]);
        }
        fwrite(buffer, sizeof(buffer[0]), n, stdout);
    }

    return 0;
}
