# About

A toy language

## Building

Requirements

- C++ compiler
- [Meson](https://mesonbuild.com/)
- [Ninja](https://ninja-build.org/)

You can download Meson + Ninja pack for Windows here: https://github.com/mesonbuild/meson/releases  
Or install it using [scoop](https://scoop.sh/)

```
> scoop install llvm ninja python
> pip install meson
```

Build and run on Windows

```
> run.bat
```

Or on other systems

```
$ meson build
$ ninja -C build
$ ./build/src/lang
```
