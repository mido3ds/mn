# Build Instructions

You will need:
- C++17 compiler
- premake5

## Windows
```
premake5 vs2017
```
You'll find the solution in the build folder

## Linux
```Bash
premake5 gmake
#You'll find the make files in the build folder
cd build
make help
#this will print all the available configurations and targets
#let's build the debug mode and release mode
make all config=debug_x64
make all config=release_x64
```