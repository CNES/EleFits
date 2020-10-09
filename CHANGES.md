# Change log

# 2.1 (breaking)

## API changes

* Naming homogeneization:
  * `ColumnInfo::repeat` -> `ColumnInfo::repeatCount`

## Bug fixes

* Column units are correctly read
* No more memory error when columns of different sizes are written

## New features

* New methods in `RecordHdu`:
  * `type` returns the HDU type (Image or Bintable)
  * `readHeader` returns the full header as a string
* New methods in `Record`:
  * `hasLongKeyword` for > 8-char keywords
  * `hasLongStringValue` for > 68-char string values
* Read bintable dimensions
* Check whether a column exists
* Program `EL_FitsIO_PrintSupportedTypes` prints supported types!
* Check whether a SifFile or MefFile is open with `FitsFile::isOpen`

## Cleaning

* `FitsFile::open` throws an error if already open
* Fixture classes better organized
* Comprehensive testing of the supported types
* Cleaning of the variadic template patterns

## Expected features

* Check whether a keyword exists
* Write COMMENT and HISTORY records

## Known bugs/issues

* `unsigned long` records larger than the greatest `long` cannot be read (CFitsIO bug), although they can be written
* Boolean columns are not supported

# 2.0 (breaking)

## Bug fixes

* Records with comments of the form `[foo] bar` are correctly written.

## API changes

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

## New features

* Records can be written as tuples in addition to parameter packs
* Records of `any` can be read and written
* New helper method `Record<T>::cast<U>` where `T` and `U` can be `any`
* Vectors of homogeneous records can be read and written
* New helper class `RecordVector` to find homogeneous records by keyword
* Method `RecordHdu::parseAllRecords` parses a whole header unit
* Minor additions:
  * `ImageHdu::readShape`

## Cleaning

* Format with clang-format
* Inline definitions in dedicated source files

## Known bugs/issues

* `unsigned long` records larger than the greatest `long` cannot be read (CFitsIO bug), although they can be written
* Boolean columns are not supported
* Columns are assumed to have consistent sizes

# 1.9.0

## API changes

* Indices and lengths as `long`s instead of `std::size_t`s

## New features

* Records can be created from `const char*` values in addition to `std::string`

## Safety

* Test CFitsIO bug in reading `unsigned long` records (see known bugs)
* Explicitely cast every conversion

## Cleaning

* Doc review

## Known bugs/issues

* `unsigned long` records larger than the greatest `long` cannot be read (CFitsIO bug), although they can be written
* Boolean columns are not supported
* Columns are assumed to have consistent sizes

## Considered changes for 2.0.0

* pascalCase functions and methods
* C++14

# 1.4.0

## Safety

* Better manage memory
* Nullify file pointer at closing
* Remove useless template specialization
* Better test type management

## Performance

* Precompilation of main template functions to make user code compile faster
* Rely on stack where possible

## Cleaning

* Fix CFitsIO include
* Overall code quality review

## Known bugs/issues

* Boolean columns are not supported
* Columns are assumed to have consistent sizes
* `const char*` is not supported; `string` should be used instead

# 1.2.1 (Patch)

* Fix row-wise reading and writing of string columns
* Fix memory issue with multiple closing of a FitsFile

# 1.2

## Bug fixes

* Record units are correctly handled
* signed char and unsigned int's are correctly handled
* Empty string records can be read
* Appending extensions in pre-existing MEF works

## New features

File handlers:

* Get the number of HDUs in a MEF file (fixes flawed behavior of CFitsIO)
* List HDU names
* Get the file name

Records:

* List record keywords
* Store records directly in user-defined structure when parsing
* Write brace-initialized records

Bintables:

* Row-wise bintable reading and writing (huge performance improvement, not available in CFitsIO)
* Append new columns in a bintable

## Considered features for 1.4

* Insert columns in a bintable
* Changes made to the record IOs (return user-defined struct) could be ported to analogous column functions (TBC)
* Functions which input parameter packs could have tuple counterparts in order to manipulate proper objects (TBC)

## Known bugs

* Boolean columns are not working

# 1.0

## Initial features

* Opening, creation, closing of MEF and SIF files
* Access to HDUs, creation of binary table and image extensions
* Reading and writing of records
* Reading and writing of binary table data
* Reading and writing of image data

## Missing features which could be added

* Bintables cannot be modified (e.g. add or remove columns or rows)
* Bintables cannot be preprocessed (sorted or filtered)
* HDUs cannot be removed
* Images cannot be (de)compressed
* Tables cannot be read by rows
* Images and tables cannot be copied

## Missing features which will probably not be added

* Iterators are not supported
* Row selection is not supported

## Known bugs

* Boolean columns are not working (requires discussions with CFitsIO devs)
* Units are not consistently handled in records
