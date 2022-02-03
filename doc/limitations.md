# Known bugs and limitations

[TOC]

## Disclaimer

The following refers to the Euclid version of CFITSIO: 3.470.
Issues may have been solved in more recent versions.

## Known CFITSIO bugs

* Cannot read `unsigned long` records if greater than `max(long)` (throws an error)
* Parsing records as `unsigned long` or `unsigned long long` values silently returns wrong values (`status == 0`)
* Image HDU type cannot be updated to `signed char` or `unsigned long long`
* Binary table columns of type `uint64_t` cannot be inserted

## Other known bugs

-

## Known CFITSIO limitations

Keyword records:
* Long comments of string records are truncated

## Unsupported CFITSIO features

HDUs:
* HDU copy

Keyword records:
* `DATE` keyword records
* Keyword record name update

Image HDUs:
* Compressed image HDUs

Table HDUs:
* ASCII tables
* Boolean binary table columns
* Multidimensional string columns
* Variable-length vector columns
* Row-wise binary table iteration
* Column type reading
* Column and row copy
* Row deletion
* Row searching and selection
* Binary table histograms

## Unsupported FITS features

* Compressed binary table HDUs
* Random groups
