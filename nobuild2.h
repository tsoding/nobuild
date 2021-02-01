#ifndef NOBUILD_H_
#define NOBUILD_H_

#ifndef _WIN32
#    define _POSIX_C_SOURCE 200809L
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <sys/stat.h>
#    include <unistd.h>
#    include <dirent.h>
#    define PATH_SEP "/"
     typedef pid_t Pid;
#else
#    define WIN32_MEAN_AND_LEAN
#    include "windows.h"
#    include <process.h>
#    define PATH_SEP "\\"
     typedef HANDLE Pid;
#endif  // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

typedef const char * Cstr;

int cstr_ends_with(Cstr cstr, Cstr postfix);
Cstr cstr_no_ext(Cstr path);

#define ENDS_WITH(cstr, postfix) cstr_ends_with(cstr, postfix)
#define NOEXT(path) cstr_no_ext(path)

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

#define CMD(...)                                    \
    do {                                            \
        Cmd_Line cmd_line = CMD_LINE(__VA_ARGS__);  \
        INFO("CMD: %s", cmd_line_show(cmd_line));   \
        cmd_line_run_sync(cmd_line);                \
    } while (0)

int path_is_dir(Cstr path);
#define IS_DIR(path) path_is_dir(path)

void path_mkdirs(Cstr_Array path);
#define MKDIRS(...)                                             \
    do {                                                        \
        Cstr_Array path = cstr_array_make(__VA_ARGS__, NULL);   \
        INFO("MKDIRS: %s", cstr_array_join(PATH_SEP, path));    \
        path_mkdirs(path);                                      \
    } while (0)

void path_rm(Cstr path);
#define RM(path)                                \
    do {                                        \
        INFO("RM: %s", path);                   \
        path_rm(path);                          \
    } while(0)

#define FOREACH_FILE_IN_DIR(file, dirpath, body)        \
    do {                                                \
        struct dirent *dp = NULL;                       \
        DIR *dir = opendir(dirpath);                    \
        if (dir == NULL) {                              \
            PANIC("could not open directory %s: %s",    \
                  dirpath, strerror(errno));            \
        }                                               \
        errno = 0;                                      \
        while ((dp = readdir(dir))) {                   \
            const char *file = dp->d_name;              \
            body;                                       \
        }                                               \
                                                        \
        if (errno > 0) {                                \
            PANIC("could not read directory %s: %s",    \
                  dirpath, strerror(errno));            \
        }                                               \
                                                        \
        closedir(dir);                                  \
    } while(0)

void VLOG(FILE *stream, Cstr tag, Cstr fmt, va_list args);
void INFO(Cstr fmt, ...);
void WARN(Cstr fmt, ...);
void ERRO(Cstr fmt, ...);
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
    memcpy(result.elems, cstrs.elems, cstrs.count * sizeof(result.elems[0]));
    result.elems[cstrs.count] = cstr;
    return result;
}

int cstr_ends_with(Cstr cstr, Cstr postfix)
{
    const size_t cstr_len = strlen(cstr);
    const size_t postfix_len = strlen(postfix);
    return postfix_len <= cstr_len
        && strcmp(cstr + cstr_len - postfix_len, postfix) == 0;
}

Cstr cstr_no_ext(Cstr path)
{
    size_t n = strlen(path);
    while (n > 0 && path[n - 1] != '.') {
        n -= 1;
    }

    if (n > 0) {
        char *result = malloc(n);
        memcpy(result, path, n);
        result[n - 1] = '\0';

        return result;
    } else {
        return path;
    }
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
    PANIC("TODO: pid_wait is not implemented for WinAPI");
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
    PANIC("TODO: cmd_line_run_sync is not implemented for WinAPI");
#else
    pid_t cpid = fork();
    if (cpid < 0) {
        PANIC("Could not fork child process: %s: %s",
              cmd_line_show(cmd_line), strerror(errno));
    }

    if (cpid == 0) {
        Cstr_Array args = cstr_array_append(cmd_line.line, NULL);

        if (execvp(args.elems[0], (char * const*) args.elems) < 0) {
            PANIC("Could not exec child process: %s: %s",
                  cmd_line_show(cmd_line), strerror(errno));
        }
    }

    return cpid;
#endif // _WIN32
}

void cmd_line_run_sync(Cmd_Line cmd_line)
{
#ifndef _WIN32
    pid_wait(cmd_line_run_async(cmd_line));
#else
    intptr_t status = _spawnvp(_P_WAIT, cmd_line.line.elems[0], (char * const*) cmd_line.line.elems);
    if (status < 0) {
        PANIC("could not start child process: %s", strerror(errno));
    }

    if (status > 0) {
        PANIC("command exited with exit code %d", status);
    }
#endif  // _WIN32
}

int path_is_dir(Cstr path)
{
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributes(path);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat statbuf = {0};
    if (stat(path, &statbuf) < 0) {
        if (errno == ENOENT) {
            return 0;
        }

        PANIC("could not retrieve information about file %s: %s",
              path, strerror(errno));
    }

    return S_ISDIR(statbuf.st_mode);
#endif // _WIN32
}

void path_mkdirs(Cstr_Array path)
{
    if (path.count == 0) {
        return;
    }

    size_t len = 0;
    for (size_t i = 0; i < path.count; ++i) {
        len += strlen(path.elems[i]);
    }

    size_t seps_count = path.count - 1;
    const size_t sep_len = strlen(PATH_SEP);

    char *result = malloc(len + seps_count * sep_len + 1);

    len = 0;
    for (size_t i = 0; i < path.count; ++i) {
        size_t n = strlen(path.elems[i]);
        memcpy(result + len, path.elems[i], n);
        len += n;

        if (seps_count > 0) {
            memcpy(result + len, PATH_SEP, sep_len);
            len += sep_len;
            seps_count -= 1;
        }

        result[len] = '\0';

        if (mkdir(result, 0755) < 0) {
            if (errno == EEXIST) {
                WARN("directory %s already exists", result);
            } else {
                PANIC("could not create directory %s: %s", result, strerror(errno));
            }
        }
    }
}

void path_rm(Cstr path)
{
    if (IS_DIR(path)) {
        FOREACH_FILE_IN_DIR(file, path, {
            if (strcmp(file, ".") != 0 && strcmp(file, "..") != 0) {
                path_rm(PATH(path, file));
            }
        });

        if (rmdir(path) < 0) {
            if (errno == ENOENT) {
                WARN("directory %s does not exist");
            } else {
                PANIC("could not remove directory %s: %s", path, strerror(errno));
            }
        }
    } else {
        if (unlink(path) < 0) {
            if (errno == ENOENT) {
                WARN("file %s does not exist");
            } else {
                PANIC("could not remove file %s: %s", path, strerror(errno));
            }
        }
    }
}

void VLOG(FILE *stream, Cstr tag, Cstr fmt, va_list args)
{
    fprintf(stream, "[%s] ", tag);
    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");
}

void INFO(Cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VLOG(stdout, "INFO", fmt, args);
    va_end(args);
}

void WARN(Cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VLOG(stderr, "WARN", fmt, args);
    va_end(args);
}

void ERRO(Cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VLOG(stderr, "ERRO", fmt, args);
    va_end(args);
}

void PANIC(Cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VLOG(stderr, "ERRO", fmt, args);
    va_end(args);
    exit(1);
}

#endif // NOBUILD_IMPLEMENTATION
