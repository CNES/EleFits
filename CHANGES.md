# Change log

# 0.9

Toward initial release (1.0)

## Initial features

* Opening, creation, closing of MEF and SIF files
* Access to HDUs, creation of binary table and image extensions
* Reading and writing of records
* Reading and writing of binary table data
* Reading and writing of image data

## Missing features

* Boolean columns are not supported (requires discussions with CFitsIO devs)
* Bintables cannot be modified (e.g. add or remove columns or rows)
* Bintables cannot be preprocessed (sorted or filtered)
* HDUs cannot be removed

## Known bugs

* Vector columns are not working except string columns (memory management issue)
* Units are not consistently handled in records
