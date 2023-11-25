# Installation and configuration guide

## Introduction

EleFits depends on Elements, a Euclid-developed build framework based on CMake.
As such, EleFits can be used either from an Elements project or from a CMake project.
Euclid developers benefit from the continuous delivery of EDEN (Euclid development environment) and have nothing to install.
Others must install EleFits (and Elements) before they configure their project.

## Download, install and test EleFits

### EDEN users

Nothing to install: all versions of EleFits are already part of EDEN.
Jump to the configuration section.

### Non-EDEN users

#### TL;DR

Download and run [the installation script](https://raw.githubusercontent.com/CNES/EleFits/5.3.0/install.sh):

```sh
wget https://raw.githubusercontent.com/CNES/EleFits/5.3.0/install.sh
chmod +x install.sh
sudo ./install.sh
```

#### More details

In the remaining, we assume that you're putting the source code of Elements and EleFits in some `<root>` directory.

You'll first need to get Elements:

```xml
cd <root>
git clone -b 5.14.0 https://github.com/astrorama/Elements.git
cd Elements
mkdir build ; cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
sudo make install
```

Finally, install EleFits the same way:

```xml
cd <root>
git clone -b 5.3.0 https://github.com/cnes/EleFits.git
cd EleFits
mkdir build ; cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_PREFIX_PATH=/usr/local ..
sudo make install
```

This will install the the set of libraries in `/usr/local/lib` (or `usr/local/lib64`), headers in `/usr/local/include`, and executables in `/usr/local/bin`.
The end-user library is named EleFits, while the low-level API is named EleCfitsioWrapper.

An install script is provided to perform all those steps, `install.sh`.
Check the help as:

```sh
install.sh -h
```

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
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(EleFits REQUIRED)
add_executable(<exe_name> <exe_source>)
target_link_libraries(<exe_name> EleFits)
```

The `find_package` command expects parent folder of the file `FindEleFits.cmake` to be in the `CMAKE_MODULE_PATH`.
For now, this file is located in the sources of EleFits, in `cmake/modules`
(we'd like it to be installed in a more standard location in some future version).
To work with the proposed `CMakeLists.txt`, it should be copied inside your project:

```sh
cd <root>/<project_name>
wget https://raw.githubusercontent.com/CNES/EleFits/5.3.0/cmake/modules/FindEleFits.cmake -P cmake/modules
mkdir build ; cd build
cmake ..
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
source ~/.bashrc
```
