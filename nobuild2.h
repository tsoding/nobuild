#ifndef NOBUILD_H_
#define NOBUILD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifndef _WIN32
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <unistd.h>
#    define PATH_SEP "/"
     typedef pid_t Pid;
#else
#    define WIN32_MEAN_AND_LEAN
#    include "windows.h"
#    define PATH_SEP "\\"
     typedef HANDLE Pid;
#endif  // _WIN32

typedef const char * Cstr;

typedef struct {
    Cstr *elems;
    size_t count;
} Cstr_Array;

Cstr_Array cstr_array_make(Cstr first, ...);
Cstr_Array cstr_array_append(Cstr_Array cstrs, Cstr cstr);
Cstr cstr_array_join(Cstr sep, Cstr_Array cstrs);

#define JOIN(sep, ...) cstr_array_join(sep, cstr_array_make(__VA_ARGS__, NULL))
#define CONCAT(...) JOIN("", __VA_ARGS__)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)

typedef struct {
    Cstr_Array line;
} Cmd_Line;

#define CMD_LINE(...) ((Cmd_Line) { .line = cstr_array_make(__VA_ARGS__, NULL) })

void pid_wait(Pid pid);
Cstr cmd_line_show(Cmd_Line cmd_line);
Pid cmd_line_run_async(Cmd_Line cmd_line);
void cmd_line_run_sync(Cmd_Line cmd_line);

void PANIC(Cstr fmt, ...);

#endif  // NOBUILD_H_

////////////////////////////////////////////////////////////////////////////////

#ifdef NOBUILD_IMPLEMENTATION

Cstr_Array cstr_array_append(Cstr_Array cstrs, Cstr cstr)
{
    Cstr_Array result = {
        .count = cstrs.count + 1
    };
    result.elems = malloc(sizeof(result.elems[0]) * result.count);
    memcpy(result.elems, cstrs.elems, cstrs.count);
    result.elems[result.count] = cstr;
    return result;
}

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

Cstr cstr_array_join(Cstr sep, Cstr_Array cstrs)
{
    if (cstrs.count == 0) {
        return "";
    }

    const size_t sep_len = strlen(sep);
    size_t len = 0;
    for (size_t i = 0; i < cstrs.count; ++i) {
        len += strlen(cstrs.elems[i]);
    }

    const size_t result_len = (cstrs.count - 1) * sep_len + len + 1;
    char *result = malloc(sizeof(char) * result_len);
    if (result == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }

    len = 0;
    for (size_t i = 0; i < cstrs.count; ++i) {
        if (i > 0) {
            memcpy(result + len, sep, sep_len);
            len += sep_len;
        }

        size_t elem_len = strlen(cstrs.elems[i]);
        memcpy(result + len, cstrs.elems[i], elem_len);
        len += elem_len;
    }
    result[len] = '\0';

    return result;
}

void pid_wait(Pid pid)
{
#ifdef _WIN32
#error "TODO: pid_wait is not implemented for WinAPI"
#else
    for (;;) {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0) {
            PANIC("could not wait on command: %s", pid, strerror(errno));
        }

        if (WIFEXITED(wstatus)) {
            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status != 0) {
                PANIC("command exited with exit code %d", exit_status);
            }

            break;
        }

        if (WIFSIGNALED(wstatus)) {
            PANIC("command process was terminated by %s", strsignal(WTERMSIG(wstatus)));
        }
    }

#endif // _WIN32
}

Cstr cmd_line_show(Cmd_Line cmd_line)
{
    // TODO: cmd_line_show does not render the command line properly
    // - No string literals when arguments contains space
    // - No escaping of special characters
    // - Etc.
    return cstr_array_join(" ", cmd_line.line);
}

Pid cmd_line_run_async(Cmd_Line cmd_line)
{
#ifdef _WIN32
#error "TODO: cmd_line_run_sync is not implemented for WinAPI"
#else
    pid_t cpid = fork();
    if (cpid < 0) {
        PANIC("Could not fork child process: %s: %s",
              cmd_line_show(cmd_line), strerror(errno));
    }

    if (cpid == 0) {
        Cstr_Array args = cstr_array_append(cmd_line.line, NULL);

        if (execvp(args.elems[0], args.elems) < 0) {
            PANIC("Could not exec child process: %s: %s",
                  cmd_line_show(cmd_line), strerror(errno));
        }
    }

    return cpid;
#endif // _WIN32
}

void cmd_line_run_sync(Cmd_Line cmd_line)
{
    pid_wait(cmd_line_run_async(cmd_line));
}

void PANIC(Cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

#endif // NOBUILD_IMPLEMENTATION
