# nobuild

Header only library for writing build recipes in C.

## Main idea

The idea is that you should not need anything but a C compiler to build a C project. No make, no cmake, no shell, no cmd, no PowerShell etc. Only C compiler. So with the C compiler you bootstrap your build system and then you use the build system to build everything else.

Try it out right here:

```
$ cc ./nobuild.c -o nobuild
$ ./nobuild
```

Explore [nobuild.c](./nobuild.c) file and the [examples](./examples) folder to learn more.

## How to use the library in your own project

Keep in mind that [nobuild.h](./nobuild.h) is an [stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) header-only library. That means that just including it does not include the implementations of the functions. You have to define `NOBUILD_IMPLEMENTATION` macro before the include. See our [nobuild.c](./nobuild.c) for an example.

1. Copy [nobuild.h](./nobuild.h) to your project
2. Create `nobuild.c` in your project with the build recipe. See our [nobuild.c](./nobuild.c) for an example.
3. Bootstrap the `nobuild` executable:
   - `$ cc nobuild.c -o nobuild` on POSIX systems
   - `$ cl.exe nobuild.c` on Windows with MSVC
4. Run the build: `$ ./nobuild`

## API Conventions

- Functions from [nobuild.h](./nobuild.h) that start with `nobuild__` (double underscore) are private to the library and should not be relied upon. They can be changed in any release without a warning.
