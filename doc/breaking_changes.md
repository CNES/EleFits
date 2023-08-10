# Breaking changes and migration guide

[TOC]

## Purpose

For each major version, this page lists the breaking changes,
details the underlying rationale,
the status in the previous version (e.g. what's there already and what's deprecated),
and how to adapt the client code.

## Expected breaking changes in 6.0

### Introduction

With the increasing amount of supported features, EleFits is no more the right place to hold advanced data designs and processing services.
Therefore, implementation of `Raster` and associated classes relies on Linx, a dedicated ND image processing library.
Many interfaces were inherited from EleFits but this still comes with a lot of breaking chages, e.g. namespaces.
Speaking of which, namespace `Euclid` is removed because it adds little value.

For integration purpose, Linx follows the standard library naming conventions for functions, which allows working with standard algorithms.
So does EleFits.

## Breaking changes in 5.0

### Introduction

The brand new feature brought by version 5.0 is the support for multidimensional columns.
This has a few impacts on the public API (e.g. the columns now have a dimension and shape),
most of which are backward compatible (but not all of them).

In order to accomodate multidimensional columns, data classes (including `Raster`) were refactored.
The refactoring brings a lot of improvements related to performance and flexibility.
Yet, most changes in the API (including the additional template parameters, see below)
won't even impact user codes.

### Many names are standardized

In `MefFile` (breaking):
* `accessFirst()` -> `find()`
* `select()` -> `filter()`

In containers, type aliases are introduced generally without breaking the API,
e.g. `value_type` is added, but `Value` is kept (though as `std::decay_t<value_type>` instead of `value_type`).

**Rationale**

Standard names are already understood by some users.

**Impact on client code**

Update names (previous names are kept for backward compatibility in this version).

### Column entries have a shape instead of repeat count

Variable `ColumnInfo::repeatCount` becomes method `ColumnInfo::repeatCount()`.

**Rationale**

The repeat count is not sufficient to describe multidimensional entries,
while having both a repeat count and shape would be unsafe.

**Impact on client code**

No impact at construction.

To read the repeat count value, use `ColumnInfo::repeatCount()` instead of `ColumnInfo::repeatCount`.

To update the repeat count, use `Column::reshape()`.

### PtrRaster constructor change

`PtrColumn(info, elementCount, data)` becomes `PtrColumn(info, rowCount, data)`.

**Rationale**

Implementation of the data classes was completely refactored for improved performance,
more flexibility and easier maintenance (see below).
This came with the merging of `PtrRaster` and `VecRaster` (which are now mere aliases)
and therefore of their constructors.

**Impact on client code**

Calls to the constructor should be updated.

### Raster and Column refactoring

`Raster<T, N>` has become `Raster<T, N, TContainer>`, where the added template parameter
allows working with any contiguous container.
`PtrRaster` and `VecRaster` are no more classes which inherit `Raster` but typedefs insead:
`PtrRaster<T, N> = Raster<T, N, T*>` and `VecRaster<T, N> = Raster<T, N, vector<T>>`.

`PtrRaster` and `VecRaster` are backward compatible.

`Column<T>` has become `Column<T, N, TContainer>`, where the added template parameters
allow supporting multidimensional entries and working with any contiguous container.
`PtrColumn` and `VecColumn` are no more classes which inherit `Column` but typedefs insead:
`PtrColumn<T, N> = Column<T, N, T*>` and `VecColumn<T, N> = Column<T, N, vector<T>>`.

The default value of `N` is 1 to limit impact on client code.

Let alone the change in `PtrColumn`'s constructor described above,
`PtrColumn` and `VecColumn` are backward compatible.

**Impact on client code**

None for basic usage.

For user-defined functions which used to return or take a `Raster` or a `Column` or a `ColumnInfo` as parameter,
e.g. `void f(Column<T>& column)`, there are several options:
* add the new template parameters,
  e.g. `void f(Column<T, N, TContainer>& column)`;
* fix the new template parameters,
  e.g. `void f(VecColumn<T>& column)`;
* use a template for the parameter itself,
  e.g. `void f(TColumn& column)`
  (which is the path taken by EleFits).

## Breaking changes in 4.0

### Introduction

Version 4.0 introduces many changes to the API.
Most of them are already in place in version 3.2, but with backward compatibility ensured by aliases.
This is done to make transition smoother on client code side.
Therefore, in 3.2, the user can use both 3.x and 4.x versions of the API.
Items removed from the API in 4.0 are marked deprecated in 3.2.

### EL_FitsIO becomes EleFits and associated changes

Namespace `FitsIO` becomes `Fits`.
Prefix `EL_Fits` (resp. `EL_FITSIO`) becomes `EleFits` (resp. `ELEFITS`).
`EL_FitsFile` becomes `EleFits`.
Underscores in program names are removed.

**Rationale**

* Underscores are forbidden by some package managers;
* Easier to say, read and write;
* "EL" stands for Euclid library, while the project is instead a package with libraries and executables;
"Ele" stands for Euclid libraries and executables.

**Status in version 3.2**

Change not applied:
compatibility with 4.0 cannot be anticipated in code which depends on 3.2.

**Impact on client code**

* Find and replace word `EL_FitsIO` with `EleFits` in CMakeLists';
* Find and replace prefix `EL_Fits` with `EleFits` in CMakeLists' and in includes;
* Find and replace prefix `EL_FITSIO` with `ELEFITS` in macros;
* Find and replace `FitsIO` (e.g. namespace `FitsIO` or class `FitsIOError`) with `Fits` in sources;
* Remove underscores from program names in command lines.

### 3rd handler level

In addition to file- and HDU-level handlers, additional classes are defined at header or data level.
For example, records are accessed through `MefFile`/`RecordHdu`/`Header` instead of `MefFile`/`RecordHdu`:

```cpp
// Until 4.0
f.access<>(3).writeRecord("KEY", 1);

// Since 4.0
f.access<>(3).header().write("KEY", 1);
```

**Rationale**

* Better mapping to FITS structure (separate header unit and data unit);
* Cleaner `SifFile` (e.g. no more useless `index()` method);
* Better application of the Single Responsibility Principle
(e.g. `BintableHdu` goes from 57 methods to 15 in `BintableColumns`);
* Better structured documentation, while many more services are provided;
* Enable row-wise binary table I/Os.

**Status in version 3.2**

Header- and data-level handlers are implemented.
Methods in HDU-level handlers still exist for backward compatibility, although they are marked deprecated.

**Impact on client code**

* Manual fix: 1 to 2 more calls in user code for each HDU;
* Negligible impact on runtime.

### RecordHdu becomes Hdu

**Rationale**

* Given the new handler level, `(Record)Hdu` is not responsible anymore for record I/Os;
* Inheritance is more explicit: `ImageHdu` and `BintableHdu` both extend `Hdu`.

**Status in version 3.2**

Name has changed.
Deprecated alias `RecordHdu` is provided for backward-compatibility.

**Impact on client code**

* Find and replace word `RecordHdu` with `Hdu`, including in include directives.

### FitsFile::Permission becomes FileMode

**Rationale**

* More classical;
* Analogous to `RecordMode`;
* Remove useless scoping, shorten usage.

**Status in version 3.2**

Name and scope have changed.
A deprecated alias `FitsFile::Permission` is provided for backward compatibility.

**Impact on client code**

* Find and replace occurrences of `FitsFile::Permission`, `SifFile::Permission` and `MefFile::Permission` with `FileMode`.

### HduType becomes HduCategory

And it is now a proper class instead of an enum.

**Rationale**

* Much richer concept now, e.g. with support for HDU state filtering;
* Enable implementation of HDU selectors, along with iterators;
* Analogous usage to `KeywordCategory`.

**Status in version 3.2**

Name has changed.
A deprecated alias `HduType` is provided for backward compatibility.

**Impact on client code**

* Find and replace occurrences of word `HduType` with `HduCategory`.

### MefFile::accessPrimary() becomes MefFile::primary() and is not a template anymore

An `ImageHdu` is always returned by `MefFile::primary()`.

**Rationale**

* The Primary is always an image;
* With the 3rd handler layer, there is no more reason to return a `RecordHdu` (see impact below).

**Status is version 3.2**

Both `MefFile::accessPrimary<T>()` and `MefFile::primary()` are implemented.

**Impact on client code**

* Replace `MefFile::accessPrimary<Hdu>()` and `MefFile::accessPrimary<ImageHdu>()` with `MefFile::access()`.

### Raster::shape and Column::info become methods

**Rationale**

Data classes should ensure data integrity as much as possible.
This is not possible with member variables.

**Status in version 3.2**

Not implemented.

**Impact on client code**

* For reading, replace `raster.shape` and `column.info` with `raster.shape()` and `column.info()`.
* The values cannot be changed after construction (at least for now).


## Breaking changes in 3.0

### All indices are 0-based

Most indices in 2.0 were 0-based (e.g. in `Raster` and `Column`).
A notable exception was the HDU index, which used to follow the CFITSIO convention.
Yet, the FITS standard doesn't set any convention for HDU indices
(well, HDU indices are not even mentioned in the standard).
Therefore, it was decided to homogeneize indexing, and follow the C++ convention instead.

For more details, see \ref types.


## Breaking changes in 2.0

### Method name change

* Method name case is now pascalCase, according to Euclid quality rules;
* Method names are homogeneized.
