# Change log

## 5.3

### Bug fixes

* `parseAll()` threw when parsing long string user-defined keywords

### Breaking changes

* `Column::entry()` deprecated and renamed as `field()` (name was wrong wrt. the FITS standard)
* Snake-cased functions names for better integration with the standard library and `Linx`
  * Pascal-cased names are kept but deprecated
  * Data classes due to be replaced with `Linx` in version 6 are unchanged

### New features

* `MefFile` supports backward indexing (e.g. `const auto& hdu = f[-2]`)
* HDUs, including the Primary, can be removed with `MefFile::remove()`
* Strategies enable and automatically perform predefined or user-defined actions, like compression or checksum validation
  * Added methods `MefFile::strategy()` and classes `Strategy`, `CompressionAction` and `Action`
  * Action `ValidateChecksums` automatically validates and possibly updates checksums
  * Action `CiteEleFits` adds a citation in the Primary (enabled by default)
  * Compression actions specify image HDU compression
* Image HDU compression
  * Added compression data classes `Quantization`, `Gzip`, `Rice`...
  * Added compression actions `Compress` and `CompressAptly`
  * Implemented compression benchmark
  * Program `EleFitsReadStructure` prints the compression type (algorithm and losslessness)
  * New program `EleFitsCompress` compresses FITS files according to the desired strategy

### Cleaning

* Previously deprecated `MefFile` members were removed

## 5.2

### Breaking changes

* Updated to Elements 6.2.1, requires C++17

### New features

* Structured bindings are supported

## 5.1

### Bug fixes

* `BintableColumns::initSeq()` was ill-formed for non-tuple sequences
* Fixed `BintableColumns::readSegmentSeq()` with unresolved bound (-1)
* Calling `BintableColumns::init()` while having accessed another HDU edited the wrong HDU

### Breaking changes

* Updated to Elements 6.1.1

## 5.0

### Bug fixes

* Function `shapeSize()` was wrong for `N = 0`
* Method `readSize()` of `ImageHdu` and `ImageRaster` was wrong for `N != 2`

### Breaking changes

* Updated to Elements 6.0.1
* `Raster<T, N>` becomes `Raster<T, N, TContainer>` to work with any contiguous container
  (with `PtrRaster<T, N> = Raster<T, N, T*>` and `VecRaster<T, N> = Raster<T, N, vector<T>>`)
* `ColumnInfo<T>` becomes `ColumnInfo<T, N>` and variable `repeatCount` becomes method `repeatCount()`
  to support multidimensional entries
* `Column<T>` becomes `Column<T, N, TContainer>`
  to support multidimensional entries and to work with any contiguous container
  (with `PtrColumn<T, N> = Column<T, N, T*>` and `VecColumn<T, N> = Column<T, N, vector<T>>`)
* `PtrColumn(info, elementCount, data)` becomes `PtrColumn(info, rowCount, data)` for homogeneity with `VecColumn`

### Refactoring

* `Raster`, `Column` and `Position` inherit `DataContainer` (see below)
* `PtrRaster`, `VecRaster`, `PtrColumn` and `VecColumn` are now mere aliases
* Methods which took a `Raster&` or `Column&` as parameter now accept any type which fullfills `Raster` or `Column` requirements
* All `DataContainer`s fulfill the standard `ContiguousContainer` requirements (e.g are iterable)
* All `DataContainer`s have vector space arithmetic (e.g. support `+`, `-`, `*`, `/`)
* `DataContainer::apply()` and `DataContainer::generate()` enable arbitrary element-wise transforms,
  including with other `DataContainer`s as arguments
* `ColumnInfo` has a member `shape` in lieu of `repeatCount` to support multidimensional entries
* `Column::entry()` returns a view as a `PtrRaster` for multidimensional columns
* `Column::elementCount()` is deprecated and replaced with standard `size()`
* `MefFile` method names have been standardized (old names are kept for backward compatibility but deprecated)
* `VecRaster::vector()` and `VecColumn::vector()` are deprecated (use `container()` instead, which performs no copy)
* `Named` and `Indexed` are deprecated (use maker function `as()`)

### Other new features

* Header units and data units can be accessed directly from a `MefFile`
  (e.g. `access<ImageRaster>(1)` is a shortcut for `access<ImageHdu>(1).raster()`)
* Multi-dimensional columns are supported
* Empty binary table HDUs can be created
* `FileMode::Write` added to edit an existing file or create a new one
* Added `BintableColumns::readSeq()` overloads for homogeneous sequences (returns `std::vector<VecColumn<T>>`)
* `Record` and `DataContainer`s support `operator<<()`
* New method `FitsFile::handoverToCfitsio()` returns the CFITSIO's `fitsfile*`
* New "Game of Life" example demonstrates the use of slicing

### Optimization and cleaning

* Access to raster and column elements is notably faster
* Many similar code blocks have been merged (e.g. through mixins)
* Several overloads have been merged (e.g. with `ColumnKey` and `TypedKey`)
* Legal notices have been reviewed
* Formatting has been reviewed
* This change log was added to Doxygen pages

## 4.0

### Renaming

* Project
  * `EL_FitsIO` -> `EleFits`
* Modules:
  * `EL_CfitsioWrapper` -> `EleCfitsioWrapper`
  * `EL_FitsUtils` -> `EleFitsUtils`
  * `EL_FitsData` -> `EleFitsData`
  * `EL_FitsFile` -> `EleFits`
  * `EL_FitsIO_Examples` -> `EleFitsExamples`
  * `EL_FitsIO_Validation` -> `EleFitsValidation`
* Programs:
  * According to module renaming, with additional underscores removed
* Namespace:
  * `FitsIO` -> `Fits`
* Macros:
  * `EL_FITSIO` prefix -> `ELEFITS`

### Other breaking changes

* Update to Elements 5.14.0
* `SifFile::header()` returns a `Header` instead of an `Hdu`
* `Raster.shape` and `Column.info` become `Raster.shape()` and `Column.info()`
* `Raster.data()`, `Column.data()` and `Column.elementCount()` are not virtual anymore:
  they call private virtual `dataImpl()` and `Column.elementCountImpl()` according to the NVI idiom
* Deprecated items are removed, most notably methods of `Hdu`, `ImageHdu` and `BintableHdu`
  which were moved to `Header`, `ImageRaster` and `BintableColumns`

## 3.2

### Bug fixes

* Column unit is written when appending a new column

### Refactoring (backward compatible)

* `RecordHdu` is renamed as `Hdu`
* New handlers `Header`, `ImageRaster` and `BintableColumns` are responsible for reading and writing header and data units
* Introduction of classes `VariantValue` to prepare migration from `boost::any` to `boost::variant` variant
* `FileMode` replaces `FitsFile::Permission`, `SifFile::Permission` and `MefFile::Permission`
* Class `HduCategory` replaces enum `HduType`
* `RecordVector` is renamed as `RecordVec`
* `FitsIOError` is renamed as `FitsError`

### New features

* HDU access
  * Named HDU access methods support `EXTVER` keyword and use HDU type to disambiguate name-version couples
  * Iterators, filters and selectors are provided to iterate over selected HDUs (e.g. with range loops)
  * HDUs are categorized more precisely with `HduCategory` (e.g. `Primary` vs. `Ext`, `Data` vs. `Metadata`) than with `HduType`
* All HDUs
  * `RecordMode` controls the behavior of record writing methods
  * HDU checksums can be computed and verified
  * `RecordSeq` is a shortcut for `RecordVec<VariantValue>`
* Image HDUs
  * Image regions can be read and written, classes `Region` and `FileMemRegions` are introduced
  * Image data can be read in place
  * `Position` has arithmetics operators
  * `makeRaster` allows bypassing template parameters to build `Raster`s
* Binary table HDUs
  * Binary table column segments can be read and written, classes `Segment` and `FileMemSegments` are introduced
  * Rows can be appended in Binary table HDUs
  * Vectors of `Column`s can be read and written, in addition to tuples
  * Binary table data can be read in place

### Deprecated

* `RecordHdu` is a deprecated alias to `Hdu`
* Methods of `Hdu`, `ImageHdu` and `BintableHdu` to read and write records, rasters and columns are deprecated:
They should be replaced with analogous methods of `Header`, `ImageRaster` and `BintableColumns`
* Explicit use of `boost::any`: should be replaced with `VariantValue`
* `FitsFile::Permission` is a deprecated alias to `FileMode`
* `HduType` is a deprecated alias to `HduCategory`
* `VecRefRaster` and `VecRefColumn`
* `RecordVector` is a deprecated alias to `RecordVec`
* `FitsIOError` is a deprecated alias to `FitsError`

### Cleaning

* `Raster` is refactored, and the value type can be const-qualified for read-only data
* `Position` is a proper class, instead of a compile-time conditional alias
* `KeywordCategory` is merged into `StandardKeyword` (named `KeywordCategory` analogously to `HduCategory`)
* Nested `Cfitsio` namespaces are renamed to disambiguate documentation
* Benchmark output is clearer
* Fuctions `seqForeach()` and `seqTransform()` loop transparently over various sequences (`vector`, `tuple`, `array`...),
  which simplifies top-level signatures

## 3.1

### Bug fixes

* String columns are correctly read when reading multiple columns
* Bitpix of compressed images is correctly parsed by `EL_FitsIO_ReadStructure`

### New features

* New method `MefFile::access(string)` checks that extactly one HDU with given name exists
* Binary table columns can be read by index
* Binary table column names can be read and updated
* `Column` values can be accessed directly with `operator()`
* `Raster` and `Column` values can be accessed with backward (negative) indices through `at` methods
* Classes `Named` and `Indexed` improve readability and safety to read records and columns
* `Raster` and `Column` define a `Value` type, `Raster` define a `Dim` constant
* Method `RecordHdu.as()` casts to another HDU handler
* Exceptions thrown all derive from `FitsIOError`
* Keywords can be filtered in `RecordHdu::readKeywords()` using new enum `KeywordCategory`

### Cleaning

* `FitsFile::open` is protected again, `FitsFile::reopen` is more robust
* HDU handlers cannot be created on their own
* Error messages contain more informations
* Benchmark is made more complete, extensible and more automated
* Major documentation update

## 3.0 (breaking)

### Breaking changes

* HDU indices are 0-based

## 2.2

### Dependency changes

* Update to Elements 5.12.0 and C++14

### Bug fixes

* Records of `any` are properly cast to records of `complex` (when valid)

### New features

* Rasters with variable dimension (`n = -1`) can be read and written
* Reaccessing an HDU doesn't prevent anymore using previous references to it
* Program `EL_FitsIO_PrintStructure` lists HDU characteristics and keywords
* File `FindEL_FitsIO.cmake` added to link against the library outside of EDEN
* Class `FitsIOError` is used to throw all exceptions
* Function `version()` returns the version of EL_FitsIO

### Cleaning

* Internal optimizations

### Expected changes for 3.0

* HDU indices will be 0-based, just like positions and other indices

## 2.1 (breaking)

### API changes

* Naming homogeneization:
  * `ColumnInfo::repeat` -> `ColumnInfo::repeatCount`

### Bug fixes

* Column units are correctly read
* No more memory error when columns of different sizes are written

### New features

* Check whether a record has a long keyword (> 8 characters) or a long string value (> 68 characters)
* Get the type of an HDU
* Read the full header as a string
* Check whether a record or column exists
* Provide a fallback value when a record is not found
* Read binary table or image dimensions
* Write a COMMENT or HISTORY record
* Check whether a SifFile or MefFile is open
* Program `EL_FitsIO_PrintSupportedTypes` prints supported types for each data class!
* `HDUNAME` keyword support for extension name

### Cleaning

* `FitsFile::open` throws an error if already open
* Fixture classes better organized
* Comprehensive testing of the supported types
* Reviewed documentation
* Cleaned variadic template patterns
* Cleaned boost::any patterns

### Known bugs/issues

* `unsigned long` records larger than the greatest `long` cannot be read (CFITSIO bug), although they can be written
* Boolean columns are not supported

## 2.0 (breaking)

### Bug fixes

* Records with comments of the form `[foo] bar` are correctly written.

### API changes

* pascalCase functions and methods
* Reading and writing operations are made explicit in method naming:
  * `ImageHdu::resize` -> `ImageHdu::updateShape`
  * `MefFile::hduNames` -> `MefFile::readHduNames`
  * `RecordHdu::keywords` -> `RecordHdu::readKeywords`
  * `RecordHdu::name` -> `RecordHdu::readName`
  * `RecordHdu::rename` -> `RecordHdu::updateName`
* Naming homogeneization:
  * `Column::nelements` -> `Column::elementCount`
  * `Column::rows` -> `Column::rowCount`

### New features

* Records can be written as tuples in addition to parameter packs
* Records of `any` can be read and written
* New helper method `Record<T>::cast<U>()` where `T` and `U` can be `any`
* Vectors of homogeneous records can be read and written
* New helper class `RecordVector` to find homogeneous records by keyword
* Method `RecordHdu::parseAllRecords` parses a whole header unit
* Minor additions:
  * `ImageHdu::readShape`

### Cleaning

* Format with clang-format
* Inline definitions in dedicated source files

### Known bugs/issues

* `unsigned long` records larger than the greatest `long` cannot be read (CFITSIO bug), although they can be written
* Boolean columns are not supported
* Columns are assumed to have consistent sizes

## 1.9

### API changes

* Indices and lengths as `long`s instead size_t`s

### New features

* Records can be created from `const char*` values in addition string`

### Safety

* Test CFITSIO bug in reading `unsigned long` records (see known bugs)
* Explicitly cast every conversion

### Cleaning

* Doc review

### Known bugs/issues

* `unsigned long` records larger than the greatest `long` cannot be read (CFITSIO bug), although they can be written
* Boolean columns are not supported
* Columns are assumed to have consistent sizes

### Considered changes for 2.0

* pascalCase functions and methods
* C++14

## 1.4

### Bug fixes

* Fix row-wise reading and writing of string columns
* Fix memory issue with multiple closing of a FitsFile

### Safety

* Better manage memory
* Nullify file pointer at closing
* Remove useless template specialization
* Better test type management

### Performance

* Precompilation of main template functions to make user code compile faster
* Rely on stack where possible

### Cleaning

* Fix CFITSIO include
* Overall code quality review

### Known bugs/issues

* Boolean columns are not supported
* Columns are assumed to have consistent sizes
* `const char*` is not supported; `string` should be used instead

## 1.2

### Bug fixes

* Record units are correctly handled
* signed char and unsigned int's are correctly handled
* Empty string records can be read
* Appending extensions in pre-existing MEF works

### New features

File handlers:

* Get the number of HDUs in a MEF file (fixes flawed behavior of CFITSIO)
* List HDU names
* Get the file name

Records:

* List record keywords
* Store records directly in user-defined structure when parsing
* Write brace-initialized records

Binary tables:

* Row-wise binary table reading and writing (huge performance improvement, not available in CFITSIO)
* Append new columns in a binary table

### Considered features for 1.4

* Insert columns in a binary table
* Changes made to the record IOs (return user-defined struct) could be ported to analogous column functions (TBC)
* Functions which input parameter packs could have tuple counterparts in order to manipulate proper objects (TBC)

### Known bugs

* Boolean columns are not working

## 1.0

### Initial features

* Opening, creation, closing of MEF and SIF files
* Access to HDUs, creation of binary table and image extensions
* Reading and writing of records
* Reading and writing of binary table data
* Reading and writing of image data

### Missing features which could be added

* Binary tables cannot be modified (e.g. add or remove columns or rows)
* Binary tables cannot be preprocessed (sorted or filtered)
* HDUs cannot be removed
* Images cannot be (de)compressed
* Tables cannot be read by rows
* Images and tables cannot be copied

### Missing features which will probably not be added

* Iterators are not supported
* Row selection is not supported

### Known bugs

* Boolean columns are not working (requires discussions with CFITSIO devs)
* Units are not consistently handled in records
