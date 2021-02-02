#ifndef NOBUILD_H_
#define NOBUILD_H_

#ifndef _WIN32
#    define _POSIX_C_SOURCE 200809L
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <sys/stat.h>
#    include <unistd.h>
#    include <dirent.h>
#    include <fcntl.h>
#    define PATH_SEP "/"
     typedef pid_t Pid;
     typedef int Fd;
#else
#    define WIN32_MEAN_AND_LEAN
#    include "windows.h"
#    include <process.h>
#    define PATH_SEP "\\"
     typedef HANDLE Pid;
     typedef HANDLE Fd;
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

#endif  // _WIN32

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define FOREACH_ARRAY(type, elem, array, body)  \
    for (size_t elem_##index = 0;                           \
         elem_##index < array.count;                        \
         ++elem_##index)                                    \
    {                                                       \
        type *elem = &array.elems[elem_##index];            \
        body;                                               \
    }

typedef const char * Cstr;

int cstr_ends_with(Cstr cstr, Cstr postfix);
#define ENDS_WITH(cstr, postfix) cstr_ends_with(cstr, postfix)

Cstr cstr_no_ext(Cstr path);
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
Pid cmd_line_run_async(Cmd_Line cmd_line, Fd *fdin, Fd *fdout);
void cmd_line_run_sync(Cmd_Line cmd_line);

typedef struct {
    Cmd_Line *elems;
    size_t count;
} Cmd_Line_Array;

#define CMD(...)                                    \
    do {                                            \
        Cmd_Line cmd_line = CMD_LINE(__VA_ARGS__);  \
        INFO("CMD: %s", cmd_line_show(cmd_line));   \
        cmd_line_run_sync(cmd_line);                \
    } while (0)

typedef enum {
    CHAIN_TOKEN_END = 0,
    CHAIN_TOKEN_IN,
    CHAIN_TOKEN_OUT,
    CHAIN_TOKEN_CMD
} Chain_Token_Type;

// A single token for the CHAIN(...) DSL syntax
typedef struct {
    Chain_Token_Type type;
    Cstr_Array args;
} Chain_Token;

#define IN(path) \
    (Chain_Token) { \
        .type = CHAIN_TOKEN_IN, \
        .args = cstr_array_make(path, NULL) \
    }

#define OUT(path) \
    (Chain_Token) { \
        .type = CHAIN_TOKEN_OUT, \
        .args = cstr_array_make(path, NULL) \
    }

#define CHAIN_CMD(...) \
    (Chain_Token) { \
        .type = CHAIN_TOKEN_CMD, \
        .args = cstr_array_make(__VA_ARGS__, NULL) \
    }

typedef struct {
    Cstr input_filepath;
    Cmd_Line_Array cmd_lines;
    Cstr output_filepath;
} Chain;

Chain chain_build_from_tokens(Chain_Token first, ...);
void chain_run_sync(Chain chain);
void chain_echo(Chain chain);

#define CHAIN(...)                                                      \
    do {                                                                \
        Chain chain = chain_build_from_tokens(__VA_ARGS__, (Chain_Token) {0}); \
        chain_echo(chain);                                              \
        chain_run_sync(chain);                                          \
    } while(0)

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

Pid cmd_line_run_async(Cmd_Line cmd_line, Fd *fdin, Fd *fdout)
{
#ifdef _WIN32
    PANIC("TODO: cmd_line_run_sync is not implemented for WinAPI");
    return 0;
#else
    pid_t cpid = fork();
    if (cpid < 0) {
        PANIC("Could not fork child process: %s: %s",
              cmd_line_show(cmd_line), strerror(errno));
    }

    if (cpid == 0) {
        Cstr_Array args = cstr_array_append(cmd_line.line, NULL);

        if (fdin) {
            if (dup2(*fdin, STDIN_FILENO) < 0) {
                PANIC("Could not setup stdin for child process: %s", strerror(errno));
            }
        }

        if (fdout) {
            if (dup2(*fdout, STDOUT_FILENO) < 0) {
                PANIC("Could not setup stdout for child process: %s", strerror(errno));
            }
        }

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
    pid_wait(cmd_line_run_async(cmd_line, NULL, NULL));
#else
    Cstr_Array args = cstr_array_append(cmd_line.line, NULL);
    intptr_t status = _spawnvp(_P_WAIT, args.elems[0], (char * const*) args.elems);
    if (status < 0) {
        PANIC("could not start child process: %s", strerror(errno));
    }

    if (status > 0) {
        PANIC("command exited with exit code %d", status);
    }
#endif  // _WIN32
}

static void chain_set_input_output_files_or_count_cmds(Chain *chain, Chain_Token token)
{
    switch (token.type) {
    case CHAIN_TOKEN_CMD: {
        chain->cmd_lines.count += 1;
    } break;

    case CHAIN_TOKEN_IN: {
        if (chain->input_filepath) {
            PANIC("Input file path was already set");
        }

        chain->input_filepath = token.args.elems[0];
    } break;

    case CHAIN_TOKEN_OUT: {
        if (chain->output_filepath) {
            PANIC("Output file path was already set");
        }

        chain->output_filepath = token.args.elems[0];
    } break;

    case CHAIN_TOKEN_END:
    default: {
        assert(0 && "unreachable");
        exit(1);
    }
    }
}

static void chain_push_cmd_line(Chain *chain, Chain_Token token)
{
    if (token.type == CHAIN_TOKEN_CMD) {
        chain->cmd_lines.elems[chain->cmd_lines.count++] = (Cmd_Line) {
            .line = token.args
        };
    }
}

Chain chain_build_from_tokens(Chain_Token first, ...)
{
    Chain result = {0};

    chain_set_input_output_files_or_count_cmds(&result, first);
    va_list args;
    va_start(args, first);
    Chain_Token next = va_arg(args, Chain_Token);
    while (next.type != CHAIN_TOKEN_END) {
        chain_set_input_output_files_or_count_cmds(&result, next);
        next = va_arg(args, Chain_Token);
    }
    va_end(args);

    result.cmd_lines.elems = malloc(sizeof(result.cmd_lines.elems[0]) * result.cmd_lines.count);
    if (result.cmd_lines.elems == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }
    result.cmd_lines.count = 0;

    chain_push_cmd_line(&result, first);

    va_start(args, first);
    next = va_arg(args, Chain_Token);
    while (next.type != CHAIN_TOKEN_END) {
        chain_push_cmd_line(&result, next);
        next = va_arg(args, Chain_Token);
    }
    va_end(args);

    return result;
}

void chain_run_sync(Chain chain)
{
#ifdef _WIN32
    PANIC("chain_run_sync is not implemented for WinAPI");
#else
    if (chain.cmd_lines.count == 0) {
        return;
    }

    Pid *cpids = malloc(sizeof(pid_t) * chain.cmd_lines.count);

    Fd pipefd[2] = {0};
    Fd fdin = 0;
    Fd *fdprev = NULL;

    if (chain.input_filepath) {
        fdin = open(chain.input_filepath, O_RDONLY);
        if (fdin < 0) {
            PANIC("could not open file %s: %s", chain.input_filepath, strerror(errno));
        }
        fdprev = &fdin;
    }

    for (size_t i = 0; i < chain.cmd_lines.count - 1; ++i) {
        if (pipe(pipefd) < 0) {
            PANIC("could not create pipe for a child process: %s", strerror(errno));
        }

        cpids[i] = cmd_line_run_async(
            chain.cmd_lines.elems[i],
            fdprev,
            &pipefd[1]);

        if (fdprev) close(*fdprev);
        close(pipefd[1]);
        fdprev = &fdin;
        fdin = pipefd[0];
    }

    {
        Fd fdout = 0;
        Fd *fdnext = NULL;

        if (chain.output_filepath) {
            fdout = open(chain.output_filepath,
                         O_WRONLY | O_CREAT | O_TRUNC,
                         S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fdout < 0) {
                PANIC("could not open file %s: %s",
                      chain.output_filepath,
                      strerror(errno));
            }
            fdnext = &fdout;
        }

        const size_t last = chain.cmd_lines.count - 1;
        cpids[last] =
            cmd_line_run_async(
                chain.cmd_lines.elems[last],
                fdprev,
                fdnext);

        if (fdprev) close(*fdprev);
        if (fdnext) close(*fdnext);
    }

    for (size_t i = 0; i < chain.cmd_lines.count; ++i) {
        pid_wait(cpids[i]);
    }
#endif // _WIN32
}

void chain_echo(Chain chain)
{
    printf("[INFO]");
    if (chain.input_filepath) {
        printf(" %s", chain.input_filepath);
    }

    FOREACH_ARRAY(Cmd_Line, cmd_line, chain.cmd_lines, {
        printf(" |> %s", cmd_line_show(*cmd_line));
    });

    if (chain.output_filepath) {
        printf(" |> %s", chain.output_filepath);
    }

    printf("\n");
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
