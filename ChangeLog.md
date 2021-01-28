[semver](https://semver.org/) is implied.

# 0.1.0-dev

*Not Released Yet*

- Introduced logging functions `INFO(fmt, ...)`, `WARN(fmt, ...)`, `ERRO(fmt, ...)`.
- `remove_ext(path)` is deprecated. Please use `NOEXT(path)` instead. `remove_ext(path)` will be removed in the next major release.
- Introduced API conventions. Functions that start with `nobuild__` (double underscore) are private to the library and should not be relied upon. They can be changed in any release without a warning.

# 0.0.1 — First Official Release

*2021-01-27*

- Implemented enough functionality to build https://github.com/tsoding/bm
