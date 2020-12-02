# Read me!

EL_FitsIO: A clean, object-oriented, template API on top of CFitsIO.

## Purpose and scope

To provide a *user-friendly and efficient* API to read and write Fits files.

The library merely consists in a CFitsIO wrapper; there is no intelligence inside.
The actual reading and writing are delegated to CFitsIO, with as few indirections as possible, to be as close as possible to CFitsIO performances.

As compared to CFitsIO API, the main benefits are those of C++ over C:

* no more raw pointers for a safe memory management
* no more type codes but template classes which handle architecture specifics for you
* no more error codes but a proper exception handling
* structures dedicated to data storage instead of zillions of unorganized and redundent variables
* a class hierarchy to represent the actual Fits organization (e.g. extensions, records, columns...) instead of the all-in-one `fitsfile` class
* a concise and clear API instead of a cluttered and verbose one
* a comprehensive testing and validation code

## Install and depend on EL_FitsIO

`EL_FitsIO` depends on `Elements`, a build framework based on CMake.
As such, `EL_FitsIO` can be used either from an Elements project or from a CMake project.

### For an Elements project

An elements project is made of modules.
The project contains a root `CMakeLists.txt`, as well as each module.
For more information, please refer to the [`Elements` documentation](https://euclid.roe.ac.uk/projects/codeen-users/wiki/User_Bui_Too).

Using the library means declaring the dependency to the `EL_FitsIO` project in the root `CMakeLists.txt`:

```xml
elements_project(<project_name> <project_version>
    USE EL_FitsIO <EL_FitsIO_version>)
```

and to the `EL_FitsFile` module (for the end-user API) in the `CMakeLists.txt` of each module which requires `EL_FitsIO`:

```xml
elements_depends_on_subdirs(EL_FitsFile)

elements_add_executable(<program_name> src/program/<program_name>.cpp
    LINK_LIBRARIES ElementsKernel EL_FitsFile)

elements_add_library(<library_name> src/Lib/*.cpp
    LINK_LIBRARIES ElementsKernel EL_FitsFile
    INCLUDE_DIRS ElementsKernel EL_FitsFile
    PUBLIC_HEADERS <library_name>)
```

### For a CMake project

You'll first need to get `Elements` 5.10.0, e.g.:

```xml
cd <root>/Elements
git checkout 5.10.0
```

Then, install `Elements` with user-defined prefix (e.g. the standard `/usr`):

```xml
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..
sudo make install -j
```

Then, install EL_FitsIO the same way:

```xml
cd <root>/EL_FitsIO
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..
sudo make install -j
```

This will install the the set of libraries in `<prefix>/lib`, headers in `<prefix>/include`, and executables in `<prefix>/bin`.
The end-user library is named `EL_FitsFile`.
Here's a minimal `CMakeLists.txt` file to use it:

```xml
CMAKE_MINIMUM_REQUIRED(VERSION <cmake_version>)
project(<project_name>)
find_package(EL_FitsFile REQUIRED)
add_executable(<exe_name> <exe_source>)
target_link_libraries(<exe_name> EL_FitsFile)
```

The `find_package` command expects parent folder of the file `FindEL_FitsFile.cmake` to be in the `CMAKE_MODULE_PATH`.
For now, it is located in the sources of `EL_FitsIO`, in `cmake/modules`.

Here's an example to configure a project:

```xml
cd <root>/<project_name>
mkdir build
cd build
cmake -DCMAKE_MODULE_PATH=<root>/EL_FitsIO/cmake/modules ..
```

## Where to go next?

To the [project home page](https://euclid.roe.ac.uk/projects/fitsio/wiki)!
There you will find up-to-date links to the hi- and low-level documentation, examples, validation programs, and so much more...
