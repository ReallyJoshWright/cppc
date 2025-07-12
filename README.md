# CPPC

This is an attempt at creating a cross platform build system for c++
written in c++.

Currently this is in alpha and makes a lot of assumptions about the user's
system. I will continue to refine things so that this will work on any system
without any assumptions.

In the future, I will have releases with the build tool compiled so that a
build step with make will not be necessary, but still available

## Current system assumptions
- You are using zsh
- You have access to c++23
- You are using g++ as the compiler
- You are on Linux
- You have make installed

## Getting started

After cloning the repo:
- run the install.sh (./install.sh)
- open a new terminal or source .zshrc (source ~/.zshrc)
- run cppc new project-name
- cd into project-name
- run cppc build
- ./app

## Issues

## Build system todo
- add run option
- add shared object creation
- add static lib creation
- add static build option
- idiot proof it (error handling)
- add clean option
- add zip option
- add tar option
- add testing option
- add verbose and -q for quiet builds

## Package manager maybe

## Cross platform todo
- add install and uninstall scripts

## Examples

### build.cpp example
```cpp

// builder.cpp
#include "helpers/include/my_builder.h"

int main() {
    Builder builder;

    Vector<Debug> debug;
    debug.push_back(Debug::G);
    debug.push_back(Debug::Wall);
    debug.push_back(Debug::Wextra);
    debug.push_back(Debug::Pedantic);

    builder.setOptions(Options{
        .name = "app",
        .root_source_file = "main.cpp",
        .version = Version::V23,
        .debug = debug,
        .optimize = Optimize::Debug,
    });

    builder.addIncludeDir("-I./include");
    builder.addIncludeDir("-I./helpers/include");
    builder.addSourceFile("./src/person.cpp");

    builder.build();
}

```

### Qt6 build.cpp example
```c++

// build.cpp
#include "my_builder.h"

int main() {
    Builder builder;

    Vector<Debug> debug;
    debug.push_back(Debug::G);
    debug.push_back(Debug::Wall);
    debug.push_back(Debug::Wextra);
    debug.push_back(Debug::Pedantic);

    builder.setOptions(Options{
        .name = "app",
        .root_source_file = "main.cpp",
        .version = Version::V23,
        .debug = debug,
        .optimize = Optimize::Debug,
    });

    builder.addIncludeDir("-I/usr/include/qt6/QtWidgets");
    builder.addIncludeDir("-I/usr/include/qt6");
    builder.addIncludeDir("-I/usr/include/qt6/QtGui");
    builder.addIncludeDir("-I/usr/include/qt6/QtCore");
    builder.addIncludeDir("-I/usr/lib/qt6/mkspecs/linux-g++");

    builder.addLibDir("-DQT_WIDGETS_LIB");
    builder.addLibDir("-DQT_GUI_LIB");
    builder.addLibDir("-DQT_CORE_LIB");

    builder.addLibrary("-lQt6Widgets");
    builder.addLibrary("-lQt6Gui");
    builder.addLibrary("-lQt6Core");

    builder.build();
}

```
