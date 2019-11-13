# EL_FitsIO

A clean, object-oriented, template API on top of CFitsIO.

## Modules

### `EL_CfitsioWrapper` is an internal CFitsIO wrapper

A set of CFitsIO functions are wrapped in C++ functions to secure memory usage, ease type handling through templates, and throw exceptions instead of returning error codes.
Yet, this API is not meant for end-users: it exposes the `fitsfile*` type and is still a bit verbose and cumbersome.

### `EL_FitsData` contains basic data-storage structures

Records, image rasters and bintable columns are implemented as light structures to abstract from CFitsIO raw pointers:

* `Record` is made of a name, value, unit and comment;
* `Raster` represents a _n_-dimensional array with constant-time pixel access;
* `Column` stores metadata (name, unit, width) and values.

### `EL_FitsFile` is the end-user API

It provides set of read-write services in dedicated classes, namely:

* `MefFile`, the root object, which provides file-level services (open, close, access HDUs);
* `RecordHdu`, the basic HDU reader-writer, which only handles records;
* `ImageHdu` and `BintableHdu`, respectively the image and bintable HDU reader-writers, which additionally provide access to the raster and columns.

### `EL_FitsIO_Examples` provide examples!

This module compares `CFitsIO`, `EL_CfitsioWrapper` and `EL_FitsFile` implementations of the same program.
It demonstrates the reading and writing of reacords, images and bintables.

### `EL_FitsIO_Validation` contains validation executables

Generators allow creating dummy Fits files from scratch.
The performances of the library are compared to those of CFitsIO.
