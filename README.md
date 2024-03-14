# Project overview

<br/>![EleFits logo](doc/diagrams/out/elefits_square.svg)

## Purpose and scope

EleFits (Euclid libraries and executables for FITS) is a modern C++ package to read and write FITS files which focuses on safety, user-friendliness, and performance.
EleFits provides the only C++ FITS library which features a fully consistent and optimal internal type management system.
Nevertheless, the API is templated and agnostic of the underlying architecture, which makes usage straightforward.
EleFits is compact and involves as few parameters as possible, which makes it both simple and robust.
As compared to CFITSIO, the benefits are mainly those of C++ over C:

* No raw pointers for a safe memory management;
* No type codes but template traits classes which handle architecture specifics;
* No error codes but a proper exception handling;
* Structures dedicated to data storage;
* A lightweight class hierarchy to represent the actual FITS organization (e.g. HDUs, records, columns).

Furthermore, exclusive features are provided, like adaptive compression, HDU selectors and automatic buffering of binary tables.

To maximize performance, EleFits is built as a CFITSIO wrapper as thin as possible.
While the two libraries are generally equivalent, optimizations implemented internally make EleFits even faster in some classical cases, unless the CFITSIO user spends considerable development efforts.

The EleFits API was specifically designed to be very fluent and compact.
The following (a bit extreme) example shows how natural it is
to read the columns named "RA" and "DEC" in the 4th extension of a Multi-Extension FITS (MEF) file:

```cpp
auto [ra, dec] = MefFile("file.fits")
    .access<BintableColumns>(4)
    .read_n(as<double>("RA"), as<double>("DEC"));
```

## Comparison to alternatives

For a more realistic use case than the toy example above, let us rely on a classical demo:
Create a Single Image FITS (SIF) file with a keyword record and an array.
Here is a comparison of EleFits with the main alternatives:

```cpp
// Given:
// - string filename: The file name
// - string keyword: The keyword record name
// - int value: The keyword record value
// - string comment: The keyword record comment
// - long width, height: The array size
// - float[width * height] data: The array values

// EleFits

SifFile fits(filename, FileMode::CREATE);
fits.header().write(keyword, value, "", comment);
fits.raster().write(make_raster(data, width, height));

// CCfits

long shape[] = { width, height };
auto fits = std::make_unique<FITS>(filename, SHORT_IMG, 2, shape);
auto& primary = fits->pHDU();
primary.addKey(name, value, comment);
primary.write(1, width * height, data);

// CFITSIO

long shape[] = { width, height };
int status = 0;
fitsfile* fits = nullptr;
fits_create_file(&fits, filename.c_str(), &status);
fits_create_img(fits, SHORT_IMG, 2, shape, &status);
fits_write_key(fits, TDOUBLE, name.c_str(), &value, comment.c_str(), &status);
fits_write_img(fits, TSHORT, 1, width * height, data, &status);
```

For completeness, here are Python options:

```py
# Astropy

header = fits.Header()
header.append((keyword, value, comment))
fits.writeto(filename, data, header)

# FITSIO

header = [{'keyword' : keyword, 'value' : value, 'comment' : comment}]
with fitsio.FITS(filename, 'rw') as fits:
    fits.write(data, header)
```

## Exclusive features

In addition, exclusive features are provided to simplify the implementation of classical tasks.
A few examples are given below.

Strategies are predefined or user-defined lists of actions to be performed automatically.
Among strategies, compression strategies enables adaptive internal compression of image HDUs:

```cpp
// Given:
// - std::string filename: A file name
// - Raster<float> data: A 2D intensity image
// - Raster<char> mask: A 2D mask image

MefFile f(filename, FileMode::Edit, CompressAuto(), ValidateChecksums());
f.append_image("", {}, image); // Automatically compresses with shuffled GZIP
f.append_image("", {}, mask); // Automatically compresses with PLIO
const auto& p = f.primary(); // Automatically validates Primary's checksums, if any
f.close(); // Automatically updates checksums of edited HDUs
```

Files are iterable, and selectors enable looping over filtered HDUs
(here, newly created image HDUs):

```cpp
// Given:
// - MefFile f: The MEF file handler
// - process_new_image: A user-defined function

for (const auto& hdu : f.filter<ImageHdu>(HduCategory::Created)) {
  process_new_image(hdu);
}
```

Heterogeneous collections of keyword records can be parsed and written,
and a comprehensive type conversion system is provided:

```cpp
// Given:
// - Header header: The header unit handler

auto records = header.parse_all(KeywordCategory::Reserved);
auto instrument = records.as<std::string>("INSTRUME");
auto exptime = records.as<double>("EXPTIME");
```

Images and tables can be read and written region-wise
by mapping -- possibly non-contiguous -- in-file and in-memory regions, e.g.:

```cpp
// Given:
// - ImageRaster raster: The image data unit handler

Box<2> region {{32, 16}, {64, 32}};
raster.read_region(region);
```

For binary tables, multiple columns can be read or written at once
to take advantage of an internal buffer:

```cpp
// Given:
// - BintableColumns columns: The binary table data unit handler

auto [col_a, col_b, col_c] = columns.read_n(
    as<char>("A"), as<double>("B"), as<std::complex<float>>("C"));
```

```cpp
// Given:
// - BintableColumns columns: The binary table data unit handler
// - Column col_a, col_b, col_c: Column containers of various value types

columns.write_n(col_a, col_b, col_c);
```

Fields of mutidimensional columns can be accessed as rasters directly:

```cpp
// Given:
// - BintableColumns columns: The binary table data unit handler

auto column = columns.read<double, 2>("THUMBNAILS");
auto raster = column.field(42);
auto pixel = raster[{3, 14}];
```

## License

[LGPL-3.0-or-later](LICENSE.md)

## Tutorial

The tutorial composed of an example program and [associated documentation](https://cnes.github.io/EleFits/5.3.1/tuto.html) is probably the good entry point for newcomers.

## Installation instructions

See [the dedicated page](INSTALL.md).

## User documentation

The [User Guide](https://cnes.github.io/EleFits/5.3.1/modules.html) is the main entry point for usage documentation.
Each so-called documentation module addresses a specific topic to learn how to use EleFits and understand why it is designed the way it is.
The API documentation of related namespaces, classes and functions is linked at the bottom of each module page.

To go further, many other topics are discussed in the [Related pages](https://cnes.github.io/EleFits/5.3.1/pages.html), as unordered documents.
Among others, you'll find there thoughts on CFITSIO, the tutorial, and some design documentation. 

## Feedbacks

Bug reports, reviews, change requests and other feedbacks are welcome!
Feel free to [open issues](https://github.com/CNES/EleFits/issues/new/choose) ;)
