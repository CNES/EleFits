# Change log

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
