# Change log

# 1.5

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
