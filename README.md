# About

Sen programming language (Work in Progress)

## Building

Requirements

- C++ compiler
- [Meson](https://mesonbuild.com/)
- [Ninja](https://ninja-build.org/)

You can download Meson + Ninja pack for Windows here: https://github.com/mesonbuild/meson/releases  
Or install it using [scoop](https://scoop.sh/)

```
> scoop install ninja python
> pip install meson
```

Build and run on Windows from VS Developer Command Prompt

```
> run release
```

Or on other systems

```
$ meson out --buildtype release
$ ninja -C out
$ ./out/src/sen
```
