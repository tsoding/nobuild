# Changelog

# [0.3.0-dev]

_Not Released Yet_

### Changed

- Have `CMD` macro use the added command system

### Added

- Define `GO_REBUILD_URSELF` macro to add code for rebuilding the executable if `nobuild.c` was modified.
- Add `rebuild_urself()` as backend for `GO_REBUILD_URSELF`.
- Add `Cstr` as type alias to `const char *`
- Define a type struct `Cstr_Array` for an array of `Cstr` that stores its length.
- Add `cstr_array_make()` for creating a `Cstr_Array` for varargs. The list must be NULL terminated.
- Add `cstr_array_append()` to create a new `Cstr_Array` and append a `Cstr` to it.
- Add `cstr_ends_with()`  function
- Add `cstr_array_join()` function
- Add `cstr_no_ext()` function
- Add `VLOG()` function that acts as the backend of each logging function.
- Add `PANIC()` function. It's like `ERRO()` but crashes the whole application.
- Add `Cmd` struct to represent a single command line.
- Add `Cmd_Array` struct to represent a series of command lines, storing the number of commands.
- Add `Fd` as type alias for File descriptor.
- Add `Pid` as type alias for process ID.
- Add `pid_wait()` to wait until the process with the specified pid is finished.
- Add `cmd_show()` to render the cmd to stdout.
- Add `cmd_run_async()` to run the `cmd` asynchronously with custom input and output.
- Add `cmd_run_sync()` to run `cmd` synchronously.
- Add `fd_open_for_read()` to open a path for reading and return a file descriptor to it.
- Add `fd_open_for_write()` to open a path for writing and return a file descriptor to it.
- Add `fd_close()` to close a file descriptor.
- Add `path_rm()` function.
- Add `path_mkdirs()` function.
- Add `path_rename()` to rename a path.
- Define `RENAME()` macro helper.
- Add `path_exists()` function.
- Define `PATH_EXISTS()` macro helper.
- Add `gcc` printf attributes to logging functions.
- Add `Pipe` struct that represents a shell pipe.
- Add `pipe_make()` function to have the os create the pipe.
- Define `PIPE()` macro helper to create a pipe for chaining commands.
- Add `Chain_Token` struct that represent a part redirectable entity.
- Add `Chain` struct that represents a series of commands piped to each other.
- Add `chain_run_sync()` to run a chain synchronously.
- Add `chain_echo()` to echo a chain.
- Add `chain_build_from_tokens()` to build a chain from tokens.
- Define `CHAIN()` macro helper to create a `Chain` from a series of `Chain_Tokens`.
- Add `Chain_Token_Type` enum to store the different types of tokens.
- Add `IN()` macro to create an input `Chain_Token`.
- Add `OUT()`  macro to create an output `Chain_Token`.
- Add `CHAIN_CMD()` to represent a command that is run as part of the chain.
- Add `shift_args()` function.

### Removed

- `PATH_SEP_LEN`
- `FOREACH_VARGS`
- `CONCAT_SEP(sep, ...) build__deprecated_concat_sep(sep, __VA_ARGS__, NULL)`
- `concat_impl(int ignore, ...)`;
- `concat_sep_impl(const char *sep, ...)`;
- `build__join(const char *sep, ...)`;
- `mkdirs_impl(int ignore, ...)`;
- `cmd_impl(int ignore, ...)`;
- `remove_ext(const char *path)`;
- `shift(int *argc, char ***argv)`;

## [0.2.0] - 2021-01-30

## Added

- Add `ENDS_WITH(str, postfix)` function that checks if `str` ends with `postfix`.
- Add `IS_DIR(path)` function that checks if `path` is a directory.
- Add `RM(path)` function that removes the specified path recursively.

# [0.1.0] - 2021-01-27

### Changed

- Deprecate `remove_ext(path)`. Use `NOEXT(path)` instead.
- Deprecate `CONCAT_SEP(sep, ...)`. Use `JOIN(sep, ...)` instead.
- Deprecate `concat_sep_impl(sep, ...)`. Use `JOIN(sep, ...)` instead.

### Added

- Add logging functions `INFO(fmt, ...)`, `WARN(fmt, ...)`, `ERRO(fmt, ...)`.
- Introduce API conventions. Functions that start with `nobuild__` (double underscore) are private to the library and should not be relied upon. They can be changed in any release without a warning.

## [0.0.1] - 2021-01-27

_Implemented enough functionality to build [bm](https://github.com/tsoding/bm.md)._
