// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// ============================================================
//
// nobuild — 0.3.0-dev — Header only library for writing build recipes in C.
//
// https://github.com/tsoding/nobuild
//
// ============================================================

#ifndef NOBUILD_H_
#define NOBUILD_H_

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif // _WIN32

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include "windows.h"
#   include <process.h>
// minirent.h HEADER BEGIN ////////////////////////////////////////
    // Copyright 2021 Alexey Kutepov <reximkut@gmail.com>
    //
    // Permission is hereby granted, free of charge, to any person obtaining
    // a copy of this software and associated documentation files (the
    // "Software"), to deal in the Software without restriction, including
    // without limitation the rights to use, copy, modify, merge, publish,
    // distribute, sublicense, and/or sell copies of the Software, and to
    // permit persons to whom the Software is furnished to do so, subject to
    // the following conditions:
    //
    // The above copyright notice and this permission notice shall be
    // included in all copies or substantial portions of the Software.
    //
    // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    // EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    // MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    // NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    // LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    // OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    // WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    //
    // ============================================================
    //
    // minirent — 0.0.1 — A subset of dirent interface for Windows.
    //
    // https://github.com/tsoding/minirent
    //
    // ============================================================
    //
    // ChangeLog (https://semver.org/ is implied)
    //
    //    0.0.1 First Official Release

    #ifndef MINIRENT_H_
    #define MINIRENT_H_

    #define WIN32_LEAN_AND_MEAN
    #include "windows.h"

    struct dirent
    {
        char d_name[MAX_PATH+1];
    };

    typedef struct DIR DIR;

    DIR *opendir(const char *dirpath);
    struct dirent *readdir(DIR *dirp);
    int closedir(DIR *dirp);

    #endif  // MINIRENT_H_
// minirent.h HEADER END ////////////////////////////////////////
#else
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <unistd.h>
#   include <dirent.h>
#   include <fcntl.h>
#endif // _WIN32


// TODO(#1): no way to disable echo in nobuild scripts
// TODO(#2): no way to ignore fails

#ifdef _WIN32
#    define PATH_SEP "\\"
#else
#    define PATH_SEP "/"
#endif // _WIN32

#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)

#define FOREACH_VARGS_CSTR(param, arg, args, body)          \
    do {                                                    \
        va_start(args, param);                              \
        for (const char *arg = va_arg(args, const char *);  \
             arg != NULL;                                   \
             arg = va_arg(args, const char *))              \
        {                                                   \
            body;                                           \
        }                                                   \
        va_end(args);                                       \
    } while(0)

#define FOREACH_VARGS(param, arg, args, body)               \
    do {                                                    \
        WARN("DEPRECATED: Please don't use FOREACH_VARGS(...). Please use FOREACH_VARGS_CSTR(...) instead. FOREACH_VARGS(...) will be removed on the next major release."); \
        va_start(args, param);                              \
        for (const char *arg = va_arg(args, const char *);  \
             arg != NULL;                                   \
             arg = va_arg(args, const char *))              \
        {                                                   \
            body;                                           \
        }                                                   \
        va_end(args);                                       \
    } while(0)

#define FOREACH_ARRAY(type, item, items, body)  \
    do {                                        \
        for (size_t i = 0;                                              \
             i < sizeof(items) / sizeof((items)[0]);                    \
             ++i)                                                       \
        {                                                               \
            type item = items[i];                                       \
            body;                                                       \
        }                                                               \
    } while(0)

#define FOREACH_FILE_IN_DIR(file, dirpath, body)        \
    do {                                                \
        struct dirent *dp = NULL;                       \
        DIR *dir = opendir(dirpath);                    \
        while ((dp = readdir(dir))) {                   \
            const char *file = dp->d_name;              \
            body;                                       \
        }                                               \
        closedir(dir);                                  \
    } while(0)

// TODO(#5): there is no way to redirect the output of CMD to a file
#define CMD(...)                                                \
    do {                                                        \
        INFO(JOIN(" ", __VA_ARGS__));                           \
        cmd_impl(69, __VA_ARGS__, NULL);                        \
    } while(0)

const char *concat_impl(int ignore, ...);
const char *concat_sep_impl(const char *sep, ...);
const char *build__join(const char *sep, ...);
int nobuild__ends_with(const char *str, const char *postfix);
int nobuild__is_dir(const char *path);
void mkdirs_impl(int ignore, ...);
void cmd_impl(int ignore, ...);
void nobuild_exec(const char **argv);
const char *remove_ext(const char *path);
char *shift(int *argc, char ***argv);
void nobuild__rm(const char *path);

#ifndef _WIN32
void nobuild__posix_wait_for_pid(pid_t pid);
#endif

typedef struct {
    const char **args;
} Cmd;

const char** nobuild_cstr_vargs_to_array(int *count, ...);


// TODO: pipes do not allow redirecting stderr
typedef struct {
    const char *input_filepath;  // NULL means stdin
    const char *output_filepath; // NULL means stdout
    Cmd *chain;
    size_t chain_size;
} Pipe;

#ifndef _WIN32
pid_t nobuild__posix_spawn_cmd(const Cmd *cmd,
                               int *fdin,  // NULL means stdin
                               int *fdout, // NULL means stdout
                               int *fderr  // NULL means stderr
                               );

void nobuild__posix_spawn_pipe(Pipe pipe);
#endif // _WIN32

typedef enum {
    PIPE_ARG_END,
    PIPE_ARG_IN,
    PIPE_ARG_OUT,
    PIPE_ARG_CHAIN,
} Pipe_Arg_Type;

typedef struct {
    Pipe_Arg_Type type;
    const char** args;
} Pipe_Arg;

Pipe nobuild__make_pipe(int ignore, ...);

#define CONCAT(...) concat_impl(69, __VA_ARGS__, NULL)
#define CONCAT_SEP(sep, ...) build__deprecated_concat_sep(sep, __VA_ARGS__, NULL)
#define JOIN(sep, ...) build__join(sep, __VA_ARGS__, NULL)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)
#define MKDIRS(...) mkdirs_impl(69, __VA_ARGS__, NULL)
#define NOEXT(path) nobuild__remove_ext(path)
#define ENDS_WITH(str, postfix) nobuild__ends_with(str, postfix)
#define IS_DIR(path) nobuild__is_dir(path)
#define RM(path)                                \
    do {                                        \
        INFO("rm %s", path);                    \
        nobuild__rm(path);                      \
    } while(0)

#ifdef _WIN32
#define PIPE(...) PANIC("Piping is not implemented on windows yet")
#else
#define PIPE(...) nobuild__posix_spawn_pipe(nobuild__make_pipe(69, __VA_ARGS__, NULL));
#endif // _WIN32a

// TODO(#17): IN and OUT are already taken by WinAPI
#define IN(path)                                                        \
    ((Pipe_Arg) {                                                       \
        .type = PIPE_ARG_IN,                                            \
        .args = nobuild_cstr_vargs_to_array(NULL, path, NULL)           \
    })

#define OUT(path)                                                       \
    ((Pipe_Arg) {                                                       \
        .type = PIPE_ARG_OUT,                                           \
        .args = nobuild_cstr_vargs_to_array(NULL, path, NULL)           \
    })

#define CHAIN(...)                                                      \
    ((Pipe_Arg) {                                                       \
        .type = PIPE_ARG_CHAIN,                                         \
        .args = nobuild_cstr_vargs_to_array(NULL, __VA_ARGS__, NULL)    \
    })

#define END                                     \
    ((Pipe_Arg) {                               \
        .type = PIPE_ARG_END                    \
    })

void nobuild_log(FILE *stream, const char *tag, const char *fmt, ...);
void nobuild_vlog(FILE *stream, const char *tag, const char *fmt, va_list args);

void INFO(const char *fmt, ...);
void WARN(const char *fmt, ...);
void ERRO(const char *fmt, ...);
void PANIC(const char *fmt, ...);

#endif  // NOBUILD_H_

#ifdef NOBUILD_IMPLEMENTATION

#ifdef _WIN32
// minirent.h IMPLEMENTATION BEGIN ////////////////////////////////////////
    struct DIR
    {
        HANDLE hFind;
        WIN32_FIND_DATA data;
        struct dirent *dirent;
    };

    DIR *opendir(const char *dirpath)
    {
        assert(dirpath);

        char buffer[MAX_PATH];
        snprintf(buffer, MAX_PATH, "%s\\*", dirpath);

        DIR *dir = (DIR*)calloc(1, sizeof(DIR));

        dir->hFind = FindFirstFile(buffer, &dir->data);
        if (dir->hFind == INVALID_HANDLE_VALUE) {
            errno = ENOSYS;
            goto fail;
        }

        return dir;

    fail:
        if (dir) {
            free(dir);
        }

        return NULL;
    }

    struct dirent *readdir(DIR *dirp)
    {
        assert(dirp);

        if (dirp->dirent == NULL) {
            dirp->dirent = (struct dirent*)calloc(1, sizeof(struct dirent));
        } else {
            if(!FindNextFile(dirp->hFind, &dirp->data)) {
                if (GetLastError() != ERROR_NO_MORE_FILES) {
                    errno = ENOSYS;
                }

                return NULL;
            }
        }

        memset(dirp->dirent->d_name, 0, sizeof(dirp->dirent->d_name));

        strncpy(
            dirp->dirent->d_name,
            dirp->data.cFileName,
            sizeof(dirp->dirent->d_name) - 1);

        return dirp->dirent;
    }

    int closedir(DIR *dirp)
    {
        assert(dirp);

        if(!FindClose(dirp->hFind)) {
            errno = ENOSYS;
            return -1;
        }

        if (dirp->dirent) {
            free(dirp->dirent);
        }
        free(dirp);

        return 0;
    }
// minirent.h IMPLEMENTATION END ////////////////////////////////////////
#endif // _WIN32

const char *build__join(const char *sep, ...)
{
    const size_t sep_len = strlen(sep);
    size_t length = 0;
    size_t seps_count = 0;

    va_list args;

    FOREACH_VARGS_CSTR(sep, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });
    assert(length > 0);

    seps_count -= 1;

    char *result = malloc(length + seps_count * sep_len + 1);

    length = 0;
    FOREACH_VARGS_CSTR(sep, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;

        if (seps_count > 0) {
            memcpy(result + length, sep, sep_len);
            length += sep_len;
            seps_count -= 1;
        }
    });

    result[length] = '\0';

    return result;
}

const char *build__deprecated_concat_sep(const char *sep, ...)
{
    WARN("DEPRECATED: Please don't use `CONCAT_SEP(sep, ...)`. Please use JOIN(sep, ...) instead. `CONCAT_SEP(sep, ...)` will be removed in the next major release.");

    const size_t sep_len = strlen(sep);
    size_t length = 0;
    size_t seps_count = 0;

    va_list args;

    FOREACH_VARGS_CSTR(sep, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });
    assert(length > 0);

    seps_count -= 1;

    char *result = malloc(length + seps_count * sep_len + 1);

    length = 0;
    FOREACH_VARGS_CSTR(sep, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;

        if (seps_count > 0) {
            memcpy(result + length, sep, sep_len);
            length += sep_len;
            seps_count -= 1;
        }
    });

    result[length] = '\0';

    return result;
}

const char *concat_sep_impl(const char *sep, ...)
{
    WARN("DEPRECATED: Please don't use `concat_sep_impl(sep, ...)`. Please use JOIN(sep, ...) instead. `concat_sep_impl(sep, ...)` will be removed in the next major release.");

    const size_t sep_len = strlen(sep);
    size_t length = 0;
    size_t seps_count = 0;

    va_list args;

    FOREACH_VARGS_CSTR(sep, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });
    assert(length > 0);

    seps_count -= 1;

    char *result = malloc(length + seps_count * sep_len + 1);

    length = 0;
    FOREACH_VARGS_CSTR(sep, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;

        if (seps_count > 0) {
            memcpy(result + length, sep, sep_len);
            length += sep_len;
            seps_count -= 1;
        }
    });

    result[length] = '\0';

    return result;
}

void mkdirs_impl(int ignore, ...)
{
    size_t length = 0;
    size_t seps_count = 0;

    va_list args;

    FOREACH_VARGS_CSTR(ignore, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });

    assert(length > 0);
    seps_count -= 1;

    char *result = malloc(length + seps_count * PATH_SEP_LEN + 1);

    length = 0;
    FOREACH_VARGS_CSTR(ignore, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;

        if (seps_count > 0) {
            memcpy(result + length, PATH_SEP, PATH_SEP_LEN);
            length += PATH_SEP_LEN;
            seps_count -= 1;
        }

        result[length] = '\0';

        INFO("mkdirs %s", result);
        if (mkdir(result, 0755) < 0) {
            if (errno == EEXIST) {
                WARN("directory %s already exists", result);
            } else {
                PANIC("could not create directory %s: %s", result, strerror(errno));
            }
        }
    });
}

// TODO(#4): there is no way to remove a file

const char *concat_impl(int ignore, ...)
{
    size_t length = 0;
    va_list args;
    FOREACH_VARGS_CSTR(ignore, arg, args, {
        length += strlen(arg);
    });

    char *result = malloc(length + 1);

    length = 0;
    FOREACH_VARGS_CSTR(ignore, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;
    });
    result[length] = '\0';

    return result;
}

void nobuild_exec(const char **argv)
{
#ifdef _WIN32
    intptr_t status = _spawnvp(_P_WAIT, argv[0], (char * const*) argv);
    if (status < 0) {
        PANIC("could not start child process: %s", strerror(errno));
    }

    if (status > 0) {
        PANIC("command exited with exit code %d", status);
    }
#else
    pid_t cpid = fork();
    if (cpid == -1) {
        PANIC("could not fork a child process: %s", strerror(errno));
    }

    if (cpid == 0) {
        if (execvp(argv[0], (char * const*) argv) < 0) {
            PANIC("could not execute child process: %s", strerror(errno));
        }
    } else {
        nobuild__posix_wait_for_pid(cpid);
    }
#endif // _WIN32
}

void cmd_impl(int ignore, ...)
{
    size_t argc = 0;

    va_list args;
    FOREACH_VARGS_CSTR(ignore, arg, args, {
        argc += 1;
    });

    const char **argv = malloc(sizeof(const char*) * (argc + 1));

    argc = 0;
    FOREACH_VARGS_CSTR(ignore, arg, args, {
        argv[argc++] = arg;
    });
    argv[argc] = NULL;

    assert(argc >= 1);

    nobuild_exec(argv);
}

const char *nobuild__remove_ext(const char *path)
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

const char *remove_ext(const char *path)
{
    WARN("DEPRECATED: Please use `NOEXT(path)` instead of `remove_ext(path)`. `remove_ext(path)` will be removed in the next major release.");
    nobuild__remove_ext(path);

    return NULL;
}

char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

void nobuild_log(FILE *stream, const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nobuild_vlog(stream, tag, fmt, args);
    va_end(args);
}

void nobuild_vlog(FILE *stream, const char *tag, const char *fmt, va_list args)
{
    fprintf(stream, "[%s] ", tag);
    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");
}

void INFO(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nobuild_vlog(stdout, "INFO", fmt, args);
    va_end(args);
}

void WARN(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nobuild_vlog(stderr, "WARN", fmt, args);
    va_end(args);
}

void ERRO(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nobuild_vlog(stderr, "ERRO", fmt, args);
    va_end(args);
}

void PANIC(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nobuild_vlog(stderr, "ERRO", fmt, args);
    va_end(args);
    exit(1);
}

int nobuild__ends_with(const char *str, const char *postfix)
{
    const size_t str_n = strlen(str);
    const size_t postfix_n = strlen(postfix);
    return postfix_n <= str_n && strcmp(str + str_n - postfix_n, postfix) == 0;
}

int nobuild__is_dir(const char *path)
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

void nobuild__rm(const char *path)
{
    if (IS_DIR(path)) {
        FOREACH_FILE_IN_DIR(file, path, {
            if (strcmp(file, ".") != 0 && strcmp(file, "..") != 0) {
                nobuild__rm(PATH(path, file));
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

Pipe nobuild__make_pipe(int ignore, ...)
{
    Pipe result = {0};

    va_list args;
    va_start(args, ignore);
    {
        Pipe_Arg arg = va_arg(args, Pipe_Arg);
        while (arg.type != PIPE_ARG_END) {
            switch (arg.type) {
            case PIPE_ARG_IN: {
                if (result.input_filepath == NULL) {
                    result.input_filepath = arg.args[0];
                } else {
                    // TODO(#15): PIPE does not report where exactly a syntactic error has happened
                    PANIC("input file was already set for the pipe");
                }
            } break;

            case PIPE_ARG_OUT: {
                if (result.output_filepath == NULL) {
                    result.output_filepath = arg.args[0];
                } else {
                    PANIC("output file was already set for the pipe");
                }
            } break;

            case PIPE_ARG_CHAIN: {
                result.chain_size += 1;
            } break;

            case PIPE_ARG_END:
            default: {
                assert(0 && "unreachable");
            }
            }

            arg = va_arg(args, Pipe_Arg);
        }
    }
    va_end(args);

    result.chain = malloc(sizeof(Cmd) * result.chain_size);

    result.chain_size = 0;
    va_start(args, ignore);
    {
        Pipe_Arg arg = va_arg(args, Pipe_Arg);
        while (arg.type != PIPE_ARG_END) {
            if (arg.type == PIPE_ARG_CHAIN) {
                result.chain[result.chain_size++].args = arg.args;
            }
            arg = va_arg(args, Pipe_Arg);
        }
    }
    va_end(args);

    return result;
}

#ifndef _WIN32
void nobuild__posix_wait_for_pid(pid_t pid)
{
    for (;;) {
        int wstatus = 0;
        waitpid(pid, &wstatus, 0);

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
}
#endif

const char** nobuild_cstr_vargs_to_array(int *out_count, ...)
{
    size_t count = 0;
    va_list args;
    FOREACH_VARGS_CSTR(out_count, arg, args, {
        count += 1;
    });

    const char **result = malloc(sizeof(const char *) * (count + 1));

    count = 0;
    FOREACH_VARGS_CSTR(out_count, arg, args, {
        result[count++] = arg;
    });

    result[count] = NULL;

    if (out_count) {
        *out_count = count;
    }

    return result;
}

#ifndef _WIN32
pid_t nobuild__posix_spawn_cmd(const Cmd *cmd,
                               int *fdin,  // NULL means stdin
                               int *fdout, // NULL means stdout
                               int *fderr  // NULL means stdout
                               )
{
    pid_t cpid = fork();

    if (cpid < 0) {
        PANIC("could not fork a child: %s", strerror(errno));
    }

    if (cpid == 0) {
        if (fdin) {
            if (dup2(*fdin, STDIN_FILENO) < 0) {
                PANIC("could not setup stdin for child process: %s", strerror(errno));
            }
        }

        if (fdout) {
            if (dup2(*fdout, STDOUT_FILENO) < 0) {
                PANIC("could not setup stdout for child process: %s", strerror(errno));
            }
        }

        if (fderr) {
            if (dup2(*fderr, STDERR_FILENO) < 0) {
                PANIC("could not setup stderr for child process: %s", strerror(errno));
            }
        }

        if (execvp(cmd->args[0], (char * const*) cmd->args) < 0) {
            PANIC("could not exec child process: %s", strerror(errno));
        }
    }

    return cpid;
}

void nobuild__posix_spawn_pipe(Pipe my_pipe)
{
    if (my_pipe.chain_size == 0) {
        return;
    }

    pid_t *cpids = malloc(sizeof(pid_t) * my_pipe.chain_size);

    int pipefd[2] = {0};
    int fdin = 0;
    int *fdprev = NULL;

    if (my_pipe.input_filepath) {
        fdin = open(my_pipe.input_filepath, O_RDONLY);
        if (fdin < 0) {
            PANIC("could not open file %s: %s", my_pipe.input_filepath, strerror(errno));
        }
        fdprev = &fdin;
    }

    for (size_t i = 0; i < my_pipe.chain_size - 1; ++i) {
        if (pipe(pipefd) < 0) {
            PANIC("could not create pipe for a child process: %s", strerror(errno));
        }

        cpids[i] = nobuild__posix_spawn_cmd(
            &my_pipe.chain[i],
            fdprev,
            &pipefd[1],
            NULL);

        if (fdprev) close(*fdprev);
        close(pipefd[1]);
        fdprev = &fdin;
        fdin = pipefd[0];
    }

    {
        int fdout = 0;
        int *fdnext = NULL;

        if (my_pipe.output_filepath) {
            fdout = open(my_pipe.output_filepath,
                         O_WRONLY | O_CREAT | O_TRUNC,
                         S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fdout < 0) {
                PANIC("could not open file %s: %s",
                      my_pipe.output_filepath,
                      strerror(errno));
            }
            fdnext = &fdout;
        }

        const size_t last = my_pipe.chain_size - 1;
        cpids[last] =
            nobuild__posix_spawn_cmd(
            &my_pipe.chain[last],
            fdprev,
            fdnext,
            NULL);

        if (fdprev) close(*fdprev);
        if (fdnext) close(*fdnext);
    }

    for (size_t i = 0; i < my_pipe.chain_size; ++i) {
        nobuild__posix_wait_for_pid(cpids[i]);
    }
}

#endif // _WIN32

#endif // NOBUILD_IMPLEMENTATION
