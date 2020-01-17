# Change log

# 0.9

Toward initial release (1.0)

## Initial features

* Opening, creation, closing of multi-extension Fits (MEF) files
* Access to HDUs, creation of binary table and image extensions
* Reading and writing of records
* Reading and writing of binary table data
* Reading and writing of image data

## Missing features

* Copy-less writing of columns
* MEF file Primary is always empty (Primary initialization not handled correctly)

## Known bugs

* Vector columns are not working except string columns (memory management issue)
* SIF files are not working (Primary initialization not handled correctly)
* Units are not consistently handled in records
