# Installation and configuration guide {#install_guide}

## Introduction

EleFits depends on Elements, a Euclid-developed build framework based on CMake.
As such, EleFits can be used either from an Elements project or from a CMake project.
Euclid developers benefit from the continuous delivery of EDEN (Euclid development environment) and have nothing to install.
Others must install EleFits (and Elements) before they configure their project.

## Install and test EleFits

### EDEN users

Nothing to install: all versions of EleFits are already part of EDEN.
Jump to the configuration section.

### Non-EDEN users

In the remaining, we assume that you're putting the source code of Elements and EleFits in some `<root>` directory.

You'll first need to get Elements:

```xml
cd <root>
git clone https://github.com/astrorama/Elements.git
cd Elements
git checkout 5.12.0
mkdir build ; cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
sudo make install
```

Finally, install EleFits the same way:

```xml
cd <root>
git clone https://github.com/cnes/EleFits.git
cd EleFits
mkdir build ; cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_PREFIX_PATH=/usr/local ..
sudo make install
```

This will install the the set of libraries in `/usr/local/lib` (or `usr/local/lib64`), headers in `/usr/local/include`, and executables in `/usr/local/bin`.
The end-user library is named EleFits, while the low-level API is named EleCfitsioWrapper.

## Configure a project to depend on EleFits

### Elements project

An Elements project is made of modules.
The project contains a root `CMakeLists.txt`, and each module contains a `CMakeLists.txt`.
For more information, please refer to the [Elements documentation](https://euclid.roe.ac.uk/projects/codeen-users/wiki/User_Bui_Too).

Using the package means declaring the dependency to the EleFits project in the root `CMakeLists.txt`:

```xml
elements_project(<project_name> <project_version>
    USE EleFits <EleFits_version>)
```

and to the EleFits module in the `CMakeLists.txt` of each module which requires EleFits:

```xml
elements_depends_on_subdirs(EleFits)

elements_add_executable(<program_name> src/program/<program_name>.cpp
    LINK_LIBRARIES ElementsKernel EleFits)

elements_add_library(<library_name> src/Lib/*.cpp
    LINK_LIBRARIES ElementsKernel EleFits
    INCLUDE_DIRS ElementsKernel EleFits
    PUBLIC_HEADERS <library_name>)
```

### CMake project

Here's a minimal `CMakeLists.txt` file to use the EleFits library (end-user API):

```xml
CMAKE_MINIMUM_REQUIRED(VERSION <cmake_version>)
project(<project_name>)
find_package(EleFits REQUIRED)
add_executable(<exe_name> <exe_source>)
target_link_libraries(<exe_name> EleFits)
```

The `find_package` command expects parent folder of the file `FindEleFits.cmake` to be in the `CMAKE_MODULE_PATH`.
For now, it is located in the sources of EleFits, in `cmake/modules`
(we'd like it to be installed in a more standard location in some future version).
Here's an example to build the project:

```xml
cd <root>/<project_name>
mkdir build ; cd build
cmake -DCMAKE_MODULE_PATH=<root>/EleFits/cmake/modules ..
make
```

Note that, to run executables, the EleFits library has to be in the library path.
This can be done, for example, with:

```xml
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64
```

which can be added to the `.bashrc`, e.g.:

```xml
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64' >> ~/.bashrc
```

For demonstration purpose, an example CMake project is provided in `EleFitsExample/doc/CMakeExample`,
which depends on EleFits and produces an executable.

It consists in:
* A `CMakeLists.txt` file which declares the dependency to EleFits and the program;
* The program source file: `HelloFits.cpp`.

To use it, first build the project as described above:

```xml
cd <root>/EleFits/EleFitsExample/doc/CMakeExample
mkdir build ; cd build
cmake -DCMAKE_MODULE_PATH=<root>/EleFits/cmake/modules ..
make
```

And to run it, don't forget to set the library path (unless already done in the `.bashrc`):

```xml
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64
./HelloFits <filename>
```

where `<filename>` is a Fits file.
