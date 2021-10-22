# Read me!

![EleFits logo](doc/diagrams/out/elefits_square.svg)

## Purpose and scope

EleFits (Euclid libraries and executables for Fits) is a modern C++ package to read and write Fits files which focuses on safety, user-friendliness, and performance.
EleFits provides the only C++ Fits library which features a fully consistent and optimal internal type management system.
Nevertheless, the API is templated and agnostic of the underlying architecture, which makes usage straightforward.
EleFits is compact and involves as few parameters as possible, which makes it both simple and robust.
As compared to CFitsIO, the benefits are mainly those of C++ over C:

* No raw pointers for a safe memory management;
* No type codes but template traits classes which handle architecture specifics;
* No error codes but a proper exception handling;
* Structures dedicated to data storage;
* A lightweight class hierarchy to represent the actual Fits organization (e.g. HDUs, records, columns).

Furthermore, exclusive features are provided, like HDU selectors and automatic buffering of binary tables.

To maximize performance, EleFits is built as a CFitsIO wrapper as thin as possible.
While the two libraries are generally equivalent, optimizations implemented internally make EleFits even faster in some classical cases, unless the CFitsIO user spends considerable development efforts.

## Example usages

Here are a few examples to demonstrate the compactness and expressivity of the library.

Create a Single Image Fits (SIF) file with a keyword record and an array:

```c++
// Given:
// - string filename: The file name
// - string name: The keyword record name
// - int value: The keyword record value
// - string comment: The keyword record comment
// - long width, height: The image size
// - float[width * height] data: The image values

SifFile f(filename, FileMode::CREATE);
f.header().write(name, value, "", comment);
f.raster().write(makeRaster(data, width, height));
```

Loop over selected HDUs (here, newly created image HDUs):

```c++
// Given:
// - MefFile f: The MEF file handler
// - processNewImage: The user-defined function

for (const auto& hdu : f.select<ImageHdu>(HduCategory::Created)) {
  processNewImage(hdu);
}
```

Parse a selection of keyword records and perform conversions:

```c++
// Given:
// - Header header: The header unit handler

auto records = header.parseAll(KeywordCategory::Reserved);
auto exptime = records.as<double>("EXPTIME");
```

Read (and write) possibly non-contiguous image data regions:

```c++
// Given:
// - ImageRaster raster: The image data unit handler
// - Raster data: The image container

Region<2> inFile { { 32, 16 }, { 64, 32 } };
Position<2> inMemory { 8, 8 };
raster.readRegionTo({ inFile, inMemory }, data);
```

Write (and read) several columns at once to optimize I/Os:

```c++
// Given:
// - BintableColumns columns: The binary table data unit handler
// - Column columnA, columnB, columnC: Column containers of various value types

columns.writeSeq(columnA, columnB, columnC);
```

## Licence

LGPL v3

## Installation instructions

See [the dedicated page](doc/INSTALL.md).

## User documentation

See [the generated documentation](https://cnes.github.io/EleFits).

## Feedbacks

Bug reports, reviews, change requests and other feedbacks are welcome!
Feel free to [open issues](https://github.com/CNES/EleFits/issues/new/choose) ;)
