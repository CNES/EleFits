# File vs. class organization

## Single Image Fits

* SIF file / `SifFile`
  * header unit / `Header`
    * records / `Record`s
  * data unit / `ImageArray`
    * array / `Raster`

## Multi Extension Fits

* MEF file / `MefFile`
  * image HDUs / `ImageHdu`s
    * header unit / `Header`
      * records / `Record`s
    * data unit / `ImageArray`
      * array / `Raster`
  * binary table HDUs / `BintableHdu`s
    * header unit / `Header`
      * records / `Record`s
    * data unit (column-wise) / `BintableColumns`
      * columns / `Column`s
    * data unit (row-wise) / `BintableRows`
      * rows / User-defined objects
