# Read me!

EL_FitsIO: A clean, object-oriented, template API on top of CFitsIO.

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

## Where to go next?

To the [project home page](https://euclid.roe.ac.uk/projects/fitsio/wiki)!
There you will find up-to-date links to the hi- and low-level documentation, examples, validation programs, and so much more...
