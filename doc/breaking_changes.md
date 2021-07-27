# Breaking changes

[TOC]

## Breaking changes in 4.0

### Introduction

Version 4.0 introduces many changes to the API.
Most of them are already in place in version 3.2, but with backward compatibility ensured by aliases.
This is done to make transition smoother on client code side.
Therefore, in 3.2, the user can use both 3.x and 4.x versions of the API.
Items removed from the API in 4.0 are marked deprecated in 3.2.

For each breaking change, this page details the underlying reasons,
the status in version 3.2,
and how to adapt the client code.

### EL_FitsIO becomes EleFits and associated changes

Namespace `FitsIO` becomes `Fits`.
Prefix `EL_Fits` becomes `EleFits`.
Underscores in program names are removed.
Namespaces are unchanged.

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
* Find and replace namespace `FitsIO` with `Fits` in sources;
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

* Better mapping to Fits structure (separate header unit and data unit);
* Cleaner `SifFile` (e.g. no more useless `index()` method);
* Better application of the Single Responsibility Principle;
* Better structured documentation, while many more services are provided;
* Enable row-wise binary table I/Os.

**Status in version 3.2**

Header- and data-level handlers are implemented.
Methods in HDU-level handlers still exist for backward compatibility, although they are marked deprecated.

**Impact on client code**

* Manual fix: 1 to 2 more calls in user code for each HDU;
* Negligible impact on runtime.

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

### MefFile::accessPrimary is not a template anymore

An `ImageHdu` is always returned.

**Rationale**

* The Primary is always an image;
* With the 3rd handler layer, there is no more reason to return a `RecordHdu` (see impact below).

**Status is version 3.2**

Not implemented:
compatibility with 4.0 cannot be anticipated in code which depends on 3.2.

**Impact on client code**

* Remove template parameter if it is `ImageHdu`;
* Use `accessPrimary().header()` instead of `accessPrimary<RecordHdu>()` when only the header unit is needed.


## Breaking changes in 3.0

### All indices are 0-based

Most indices in 2.0 were 0-based (e.g. in `Raster` and `Column`).
A notable exception was the HDU index, which used to follow the CFitsIO convention.
Yet, the Fits standard doesn't set any convention for HDU indices
(well, HDU indices are not even mentioned in the standard).
Therefore, it was decided to homogeneize indexing, and follow the C++ convention instead.

For more details, see \ref types.


## Breaking changes in 2.0

### Method name change

* Method name case is now pascalCase, according to Euclid quality rules;
* Method names are homogeneized.
