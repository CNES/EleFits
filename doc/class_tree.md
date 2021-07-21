# File vs. class organization

## Single Image Fits

* SIF file / `SifFile`
  * header unit / `Header` <-> `Record`s
  * data unit / `ImageRaster` <-> `Raster`

## Multi Extension Fits

* MEF file = `MefFile`
  * image HDUs = `ImageHdu`s
    * header unit = `Header` <-> `Record`s
    * data unit = `ImageRaster` <-> `Raster`
  * binary table HDUs = `BintableHdu`s
    * header unit = `Header` <-> `Record`s
    * data unit = `BintableColumns` / `BintableRows` <-> `Column`s / User-defined objects
