# Installation and configuration guide {#install_guide}

## Introduction

EL_FitsIO depends on Elements, a build framework based on CMake.
As such, EL_FitsIO can be used either from an Elements project or from a CMake project.
Euclid developers benefit from the continuous delivery of EDEN (Euclid development environment) and have nothing to install.
Others must install EL_FitsIO (and Elements) before they configure their project.

## Install and test EL_FitsIO

### EDEN users

Nothing to install: all versions of EL_FitsIO are already part of EDEN.
Jump to the configuration section.

### Non-EDEN users

In the remaining, we assume that you're putting the source code of Elements and EL_FitsIO in some `<root>` directory.

You'll first need to get Elements:

```xml
cd <root>
git clone https://github.com/astrorama/Elements.git
cd Elements
git checkout <Elements_version>
```

Then, install Elements with user-defined prefix (e.g. the standard `/usr`):

```xml
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..
sudo make install -j
```

Finally, install EL_FitsIO the same way:

```xml
cd <root>/EL_FitsIO
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..
make -j
make test
sudo make install -j
```

This will install the the set of libraries in `<prefix>/lib`, headers in `<prefix>/include`, and executables in `<prefix>/bin`.
The end-user library is named EL_FitsFile, while the low-level API is named EL_CfitsioWrapper.

## Configure a project to depend on EL_FitsIO

### Elements project

An Elements project is made of modules.
The project contains a root `CMakeLists.txt`, and each module contains a `CMakeLists.txt`.
For more information, please refer to the [Elements documentation](https://euclid.roe.ac.uk/projects/codeen-users/wiki/User_Bui_Too).

Using the package means declaring the dependency to the EL_FitsIO project in the root `CMakeLists.txt`:

```xml
elements_project(<project_name> <project_version>
    USE EL_FitsIO <EL_FitsIO_version>)
```

and to the EL_FitsFile module (for the end-user API) or to the EL_CfitsioWrapper module (for the low-level API)
in the `CMakeLists.txt` of each module which requires EL_FitsIO:

```xml
elements_depends_on_subdirs(EL_FitsFile)

elements_add_executable(<program_name> src/program/<program_name>.cpp
    LINK_LIBRARIES ElementsKernel EL_FitsFile)

elements_add_library(<library_name> src/Lib/*.cpp
    LINK_LIBRARIES ElementsKernel EL_FitsFile
    INCLUDE_DIRS ElementsKernel EL_FitsFile
    PUBLIC_HEADERS <library_name>)
```

### CMake project

Here's a minimal `CMakeLists.txt` file to use the EL_FitsFile library (end-user API):

```xml
CMAKE_MINIMUM_REQUIRED(VERSION <cmake_version>)
project(<project_name>)
find_package(EL_FitsFile REQUIRED)
add_executable(<exe_name> <exe_source>)
target_link_libraries(<exe_name> EL_FitsFile)
```

To use the low-level API, replace EL_FitsFile with EL_CfitsioWrapper.

The `find_package` command expects parent folder of the file `FindEL_FitsFile.cmake` to be in the `CMAKE_MODULE_PATH`.
For now, it is located in the sources of EL_FitsIO, in `cmake/modules`
(we'd like it to be installed in a more standard location in some future version).
Here's an example to build the project:

```xml
cd <root>/<project_name>
mkdir build
cd build
cmake -DCMAKE_MODULE_PATH=<root>/EL_FitsIO/cmake/modules ..
```
