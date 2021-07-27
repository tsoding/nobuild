# nobuild

Header only library for writing build recipes in C.

## Main idea

The idea is that you should not need anything but a C compiler to build a C project. No make, no cmake, no shell, no cmd, no PowerShell etc. Only C compiler. So with the C compiler you bootstrap your build system and then you use the build system to build everything else.

Try it out right here:

```console
$ cc ./nobuild.c -o nobuild
$ ./nobuild
```

Explore [nobuild.c](./nobuild.c) file and the [examples](./examples) folder to learn more.

## This is an Experimental Project

I'm not sure if this is even a good idea myself. This is why I'm implementing it. This is a research project. I'm not making any claims about suitability of this approach to any project.

Right now I'm actively using nobuild only in [bm](https://github.com/tsoding/bm). It works quite well for me there.

## It's likely Not Suitable for Your Project

If you are using [cmake](https://cmake.org/) with tons of modules to manage and find tons of dependencies you probably don't want to use this tool. nobuild is more like writting shell scripts but in C.

## Advantages of nobuild

- Extremely portable builds across variety of systems including (but not limited to) Linux, MacOS, Windows, FreeBSD, etc. This is achieved by reducing the amount of dependencies to just a C compiler, which exists pretty much for any platform these days.
- You end up using the same language for developing and building your project. Which may enable some interesting code reusage strategies. The build system can use the code of the project itself directly and the project can use the code of the build system also directly.
- You get to use C more.
- ...

## Disadvantages of nobuild

- You need to be comfortable with C and implementing things yourself. As mentioned above this is like writing shell scripts but in C.
- It probably does not make any sense outside of C/C++ projects.
- You get to use C more.
- ...

## Why is it called "nobuild" when it's clearly a build tool?

You know all these BS movements that supposedly remove the root cause of your problems? Things like [NoSQL](https://en.wikipedia.org/wiki/NoSQL), [No-code](https://en.wikipedia.org/wiki/No-code_development_platform), [Serverless](https://en.wikipedia.org/wiki/Serverless_computing), etc. This is the same logic. I had too many problems with the process of building C projects. So there is nobuild anymore.

## How to use the library in your own project

Keep in mind that [nobuild.h](./nobuild.h) is an [stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) header-only library. That means that just including it does not include the implementations of the functions. You have to `#define NOBUILD_IMPLEMENTATION` before the include. See our [nobuild.c](./nobuild.c) for an example.

1. Copy [nobuild.h](./nobuild.h) to your project
2. Create `nobuild.c` in your project with the build recipe. See our [nobuild.c](./nobuild.c) for an example.
3. Bootstrap the `nobuild` executable:
   - `$ cc nobuild.c -o nobuild` on POSIX systems
   - `$ cl.exe nobuild.c` on Windows with MSVC
4. Run the build: `$ ./nobuild`

If you enable the [Go Rebuild Urself™](https://github.com/tsoding/nobuild/blob/d2bd711f0e2bcff0651850cd795509ab104ad9d4/nobuild.h#L218-L239) Technology the `nobuild` executable will try to rebootstrap itself every time you modify its source code.
