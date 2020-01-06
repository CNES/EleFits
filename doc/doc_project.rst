Project Overview
================


Introduction
------------

A template library to read and write Fits files efficiently.


Rationale
---------

There are two official libraries to work with Fits files: `CFitsIO` and `CCfits`.

Their APIs are both very noisy and error-prone.
`EL_FitsIO` exposes a clean C++ API to the user, and has performances similar to CFitsIO.

`CFitsIO` has a painful C low-level API.
It is very verbose due to the use of output parameters and an amazing parametrization level
(which we do not target with `EL_FitsIO`).
It also uses deprecated C++ features (const-incorrectness, strings as char*...)
and most importantly relies on a flawed type management,
where C++ types (e.g. `short`, `int`) and fixed-size types (`int16_t`, `int32_t`) interact (and collide!) silently.

`CCfits`, on the other hand, is very slow (like, 10 to 100x slower than `CFitsIO`).
It provides some template functions but still with `CFitsIO` type codes,
some classes but still with a comfortable amount of raw pointers,
a mostly new API but which sparsely exposes the CFitsIO API, and so on.

All in all, none of those solutions are appealing to C++ users.


Purpose and Scope
-----------------

The purpose of the library is to provide a user-friendly and efficient API to read and write Fits files.

The library merely consists in a thin `CFitsIO` wrapper:
There is no intelligence inside.
The actual reading and writing are delegated to `CFitsIO`,
with as few indirections as possible to minimize overhead.
As compared to `CFitsIO` API, the main benefits are:

* No more raw pointers for a safe memory management;
* No more type codes but template classes which handle architecture specifics for you;
* No more error codes but proper exception handling;
* Structures dedicated to data storage instead of zillions of unorganized and redundent variables;
* A lightweight class hierarchy to represent the actual Fits organization (e.g. extensions, records, columns...)
instead of the all-in-one and flat fitsfile structure;
* A concise and clear API instead of the cluttered and verbose one of `CFitsIO`;
* A comprehensive testing and validation code.

Simplicity comes at the cost of completeness,
and `EL_FitsIO` misses `CFitsIO` features that we found less important or too complex,
like ASCII table extensions, streaming or compression.


Architecture overview
---------------------

The library is split in three modules:

* EL_FitsData contains the raw structures to store Fits data (records, image rasters, and table columns).
* EL_CfitsioWrapper is a set of low-level functions to call Cfitsio with modern C++ signatures.
* EL_FitsFile is an object-oriented API on top of the latter module;
It is therefore recommended to avoid relying directly on the EL_CfitsioWrapper API;
There is no data storage in the classes, so that overhead is minimal.

To make it simple, the end-user API is gathered in the FitsIO namespace,
where both data-storage and service classes are implemented.


Modules
-------

EL_CfitsioWrapper is an internal CFitsIO wrapper
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A set of CFitsIO functions are wrapped in C++ functions to secure *memory usage*, ease *type handling* through templates, and throw *exceptions* instead of returning error codes.
Yet, this API is not meant for end-users: it exposes the `fitsfile*` type and is still a bit verbose and cumbersome.

EL_FitsData contains basic data-storage structures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Records, image rasters and bintable columns are implemented as *light structures* to abstract from CFitsIO raw pointers:

* `Record` is made of a name, value, unit and comment;
* `Raster` represents a _n_-dimensional array with constant-time pixel access;
* `Column` stores metadata (name, unit, width) and values.

EL_FitsFile is the end-user API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It provides set of *read-write services* in dedicated classes, namely:

* `MefFile`, the root object, which provides file-level services (open, close, access HDUs);
* `RecordHdu`, the basic HDU reader-writer, which only handles records;
* `ImageHdu` and `BintableHdu`, respectively the image and bintable HDU reader-writers, which additionally provide access to the raster and columns.

EL_FitsIO_Examples provides examples!
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This module compares `CFitsIO`, `EL_CfitsioWrapper` and `EL_FitsFile` implementations of the same program.
It demonstrates the reading and writing of records, images and bintables.

EL_FitsIO_Validation contains validation executables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Generators allow creating dummy Fits files from scratch.
The performances of the library are compared to those of CFitsIO.
