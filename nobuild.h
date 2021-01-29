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
// nobuild — 0.2.0-dev — Header only library for writing build recipes in C.
//
// https://github.com/tsoding/nobuild
//
// ============================================================

#ifndef NOBUILD_H_
#define NOBUILD_H_

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
#endif // _WIN32

// TODO(#1): no way to disable echo in nobuild scripts
// TODO(#2): no way to ignore fails

#ifdef _WIN32
#    define PATH_SEP "\\"
#else
#    define PATH_SEP "/"
#endif // _WIN32

#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)

#define FOREACH_VARGS(param, arg, args, body)               \
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

#define CONCAT(...) concat_impl(69, __VA_ARGS__, NULL)
#define CONCAT_SEP(sep, ...) build__deprecated_concat_sep(sep, __VA_ARGS__, NULL)
#define JOIN(sep, ...) build__join(sep, __VA_ARGS__, NULL)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)
#define MKDIRS(...) mkdirs_impl(69, __VA_ARGS__, NULL)
#define NOEXT(path) nobuild__remove_ext(path)
#define ENDS_WITH(str, postfix) nobuild__ends_with(str, postfix)
#define IS_DIR(path) nobuild__is_dir(path)

void nobuild_log(FILE *stream, const char *tag, const char *fmt, ...);
void nobuild_vlog(FILE *stream, const char *tag, const char *fmt, va_list args);

void INFO(const char *fmt, ...);
void WARN(const char *fmt, ...);
void ERRO(const char *fmt, ...);
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

    FOREACH_VARGS(sep, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });
    assert(length > 0);

    seps_count -= 1;

    char *result = malloc(length + seps_count * sep_len + 1);

    length = 0;
    FOREACH_VARGS(sep, arg, args, {
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

    FOREACH_VARGS(sep, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });
    assert(length > 0);

    seps_count -= 1;

    char *result = malloc(length + seps_count * sep_len + 1);

    length = 0;
    FOREACH_VARGS(sep, arg, args, {
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

    FOREACH_VARGS(sep, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });
    assert(length > 0);

    seps_count -= 1;

    char *result = malloc(length + seps_count * sep_len + 1);

    length = 0;
    FOREACH_VARGS(sep, arg, args, {
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

    FOREACH_VARGS(ignore, arg, args, {
        length += strlen(arg);
        seps_count += 1;
    });

    assert(length > 0);
    seps_count -= 1;

    char *result = malloc(length + seps_count * PATH_SEP_LEN + 1);

    length = 0;
    FOREACH_VARGS(ignore, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;

        if (seps_count > 0) {
            memcpy(result + length, PATH_SEP, PATH_SEP_LEN);
            length += PATH_SEP_LEN;
            seps_count -= 1;
        }

        result[length] = '\0';

        INFO("mkdir %s", result);
        if (mkdir(result, 0755) < 0) {
            if (errno == EEXIST) {
                WARN("directory %s already exists", result);
            } else {
                ERRO("could not create directory %s: %s", result, strerror(errno));
                exit(1);
            }
        }
    });
}

// TODO(#3): there is no way to remove a folder
// TODO(#4): there is no way to remove a file

const char *concat_impl(int ignore, ...)
{
    size_t length = 0;
    va_list args;
    FOREACH_VARGS(ignore, arg, args, {
        length += strlen(arg);
    });

    char *result = malloc(length + 1);

    length = 0;
    FOREACH_VARGS(ignore, arg, args, {
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
        ERRO("could not start child process: %s", strerror(errno));
        exit(1);
    }

    if (status > 0) {
        ERRO("command exited with exit code %d", status);
        exit(1);
    }
#else
    pid_t cpid = fork();
    if (cpid == -1) {
        ERRO("could not fork a child process: %s", strerror(errno));
        exit(1);
    }

    if (cpid == 0) {
        if (execvp(argv[0], (char * const*) argv) < 0) {
            ERRO("could not execute child process: %s", strerror(errno));
            exit(1);
        }
    } else {
        for (;;) {
            int wstatus = 0;
            wait(&wstatus);

            if (WIFEXITED(wstatus)) {
                int exit_status = WEXITSTATUS(wstatus);
                if (exit_status != 0) {
                    ERRO("command exited with exit code %d", exit_status);
                    exit(1);
                }

                break;
            }

            if (WIFSIGNALED(wstatus)) {
                ERRO("command process was terminated by signal %d", WTERMSIG(wstatus));
                exit(1);
            }
        }
    }
#endif // _WIN32
}

void cmd_impl(int ignore, ...)
{
    size_t argc = 0;

    va_list args;
    FOREACH_VARGS(ignore, arg, args, {
        argc += 1;
    });

    const char **argv = malloc(sizeof(const char*) * (argc + 1));

    argc = 0;
    FOREACH_VARGS(ignore, arg, args, {
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

        ERRO("could not retrieve information about file %s: %s",
             path, strerror(errno));
        exit(1);
    }

    return (statbuf.st_mode & S_IFMT) == S_IFDIR;
#endif // _WIN32
}

#endif // NOBUILD_IMPLEMENTATION
