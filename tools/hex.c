#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#define COLUMNS 16
#define PADDING 6

uint8_t buffer[COLUMNS];

int main(void)
{
    while (!feof(stdin)) {
        size_t n = fread(buffer, sizeof(buffer[0]), COLUMNS, stdin);

        for (size_t i = 0; i < n; ++i) {
            printf("%02X ", buffer[i]);
            if (!isprint(buffer[i])) buffer[i] = '.';
        }

        assert(n <= COLUMNS);
        printf("%*s", (int) (PADDING + (COLUMNS - n) * 3), "");

        fwrite(buffer, sizeof(buffer[0]), n, stdout);
        printf("\n");
    }

    return 0;
}
