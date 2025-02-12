# nobuild

Header only library for writing build recipes in C.

> [!WARNING]
> This library is DEPRECATED! Use [https://github.com/tsoding/nob.h](https://github.com/tsoding/nob.h) instead

## How to use the library in your own project

Keep in mind that [nobuild.h](./nobuild.h) is an [stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) header-only library. That means that just including it does not include the implementations of the functions. You have to `#define NOBUILD_IMPLEMENTATION` before the include. See our [nobuild.c](./nobuild.c) for an example.

1. Copy [nobuild.h](./nobuild.h) to your project
2. Create `nobuild.c` in your project with the build recipe. See our [nobuild.c](./nobuild.c) for an example.
3. Bootstrap the `nobuild` executable:
   - `$ cc nobuild.c -o nobuild` on POSIX systems
   - `$ cl.exe nobuild.c` on Windows with MSVC
4. Run the build: `$ ./nobuild`

If you enable the [Go Rebuild Urself™](https://github.com/tsoding/nobuild/blob/d2bd711f0e2bcff0651850cd795509ab104ad9d4/nobuild.h#L218-L239) Technology the `nobuild` executable will try to rebootstrap itself every time you modify its source code.
