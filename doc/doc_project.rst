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
It is very verbose due to the use of output parameters and a crazy parametrization level (which we do not target with `EL_FitsIO`).
It also uses deprecated C++ features (const-incorrectness, char* strings...) and relies on a flawed type management,
where C++ types and fixed-size types interact quite silently.

`CCfits`, on the other hand, is very slow (like, 100x slower than `CFitsIO`).
It provides some templated functions but still with `CFitsIO` type codes,
some classes but still with a comfortable amount of raw pointers,
a mostly new API but which sparsily exposes the CFitsIO API

All in all, none of those solutions are appealing to C++ users.


`EL_FitsIO` strengths
---------------------

Just a thin layer on top of `CFitsIO` with minimal overhead (see performance tests):

* Clean API: Template object-oriented C++11
* No type codes, no error codes, no pointers...


`EL_FitsIO` drawbacks
---------------------

Not full-featured:
* No ASCII table extensions
* No streaming
* No compression...


Architecture overview
---------------------

The project is split in three modules:

* EL_FitsData contains the raw structures to store Fits data (records, image rasters, and table columns).
* EL_CfitsioWrapper is a set of low-level functions to call Cfitsio with modern C++ signatures.
* EL_FitsFile is an object-oriented API on top of the latter module;
It is therefore recommended to avoid relying directly o the EL_CfitsioWrapper API;
There is no data storage in the classes, so that overhead is minimal.
