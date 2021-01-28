[semver](https://semver.org/) is implied.

# 0.1.0 — Cleaning Things Up. Figuring Out Conventions.

*2021-01-27*

- Introduced logging functions `INFO(fmt, ...)`, `WARN(fmt, ...)`, `ERRO(fmt, ...)`.
- Introduced API conventions. Functions that start with `nobuild__` (double underscore) are private to the library and should not be relied upon. They can be changed in any release without a warning.

## Deprecations

All the deprecated functions and macros will be removed in the next major release.

- `remove_ext(path)` is deprecated. Use `NOEXT(path)` instead.
- `CONCAT_SEP(sep, ...)` is deprecated. Use `JOIN(sep, ...)` instead.
- `concat_sep_impl(sep, ...)` is deprecated. Use `JOIN(sep, ...)` instead.

# 0.0.1 — First Official Release

*2021-01-27*

- Implemented enough functionality to build https://github.com/tsoding/bm
