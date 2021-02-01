#ifndef NOBUILD_H_
#define NOBUILD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

typedef const char * Cstr;

typedef struct {
    Cstr *elems;
    size_t count;
} Cstr_Array;

Cstr_Array cstr_array_make(Cstr first, ...);

void PANIC(Cstr fmt, ...);

#endif  // NOBUILD_H_

////////////////////////////////////////////////////////////////////////////////
#ifdef NOBUILD_IMPLEMENTATION

Cstr_Array cstr_array_make(Cstr first, ...)
{
    Cstr_Array result = {0};

    if (first == NULL) {
        return result;
    }

    result.count += 1;

    va_list args;
    va_start(args, first);
    for (Cstr next = va_arg(args, Cstr);
         next != NULL;
         next = va_arg(args, Cstr))
    {
        result.count += 1;
    }
    va_end(args);

    result.elems = malloc(sizeof(result.elems[0]) * result.count);
    if (result.elems == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }
    result.count = 0;
    
    result.elems[result.count++] = first;

    va_start(args, first);
    for (Cstr next = va_arg(args, Cstr);
         next != NULL;
         next = va_arg(args, Cstr))
    {
        result.elems[result.count++] = next;
    }
    va_end(args);

    return result;
}

void PANIC(Cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

#endif // NOBUILD_IMPLEMENTATION
