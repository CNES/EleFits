# EL_FitsIO

A clean, object-oriented, template API on top of CFitsIO.

## Purpose and scope

To provide a *user-friendly and efficient* API to read and write Fits files.

The library merely consists in a CFitsIO wrapper; there is no intelligence inside.
The actual reading and writing are delegated to CFitsIO, with as few indirections as possible, to be as close as possible to CFitsIO performances.

As compared to CFitsIO API, the main benefits are:

* no more raw pointers for a safe memory management
* no more type codes but template classes which handle architecture specifics for you
* no more error codes but a proper exception handling
* structures dedicated to data storage instead of zillions of unorganized and redundent variables
* a class hierarchy to represent the actual Fits organization (e.g. extensions, records, columns...) instead of the all-in-one `fitsfile` class
* a concise and clear API instead of a cluttered and verbose one
* a comprehensive testing and validation code

## Modules

### `EL_CfitsioWrapper` is an internal CFitsIO wrapper

A set of CFitsIO functions are wrapped in C++ functions to secure *memory usage*, ease *type handling* through templates, and throw *exceptions* instead of returning error codes.
Yet, this API is not meant for end-users: it exposes the `fitsfile*` type and is still a bit verbose and cumbersome.

### `EL_FitsData` contains basic data-storage structures

Records, image rasters and bintable columns are implemented as *light structures* to abstract from CFitsIO raw pointers:

* `Record` is made of a name, value, unit and comment;
* `Raster` represents a _n_-dimensional array with constant-time pixel access;
* `Column` stores metadata (name, unit, width) and values.

### `EL_FitsFile` is the end-user API

It provides set of *read-write services* in dedicated classes, namely:

* `MefFile`, the root object, which provides file-level services (open, close, access HDUs);
* `RecordHdu`, the basic HDU reader-writer, which only handles records;
* `ImageHdu` and `BintableHdu`, respectively the image and bintable HDU reader-writers, which additionally provide access to the raster and columns.

### `EL_FitsIO_Examples` provide examples!

This module compares `CFitsIO`, `EL_CfitsioWrapper` and `EL_FitsFile` implementations of the same program.
It demonstrates the reading and writing of records, images and bintables.

### `EL_FitsIO_Validation` contains validation executables

Generators allow creating dummy Fits files from scratch.
The performances of the library are compared to those of CFitsIO.

## Where to go next?

* [Project home page](https://euclid.roe.ac.uk/projects/fitsio/wiki)
* [Example programs and comparison to CFitsIO](EL_FitsIO_Examples/cpp/programs)
* [Full API documentation](TODO)
