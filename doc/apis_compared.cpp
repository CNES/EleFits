#include "EL_FitsFile/SifFile.h"

#include <fitsio.h>
#include <memory>
#include <string>

using namespace Euclid::FitsIO;

long width = 300;
long height = 200;
std::string filename = "testfile.fits";
std::string name = "EXPOSURE";
double value = 1500;
std::string comment = "Total Exposure Time";
short data[width * height]; // TODO

// CFITSIO: 8 lines; 280 characters; 14 parameters (excluding fptr and status)

void cfitsioExample() {
  long shape[] = { width, height };
  int status = 0;
  fitsfile* fptr = nullptr;
  fits_create_file(&fptr, filename.c_str(), &status);
  fits_create_img(fptr, SHORT_IMG, 2, shape, &status);
  fits_write_key(fptr, TDOUBLE, name.c_str(), &value, comment.c_str(), &status);
  fits_write_img(fptr, TSHORT, 1, width * height, data, &status);
}

// CCfits: 5 lines; 180 characters; 12 parameters

void ccfitsExample() {
  long shape[] = { width, height };
  auto pFits = std::make_unique<FITS>(filename, SHORT_IMG, 2, shape);
  PHDU& primary = pFits->pHDU();
  primary.addKey(name, value, comment);
  primary.write(1, width * height, data);
}

// SFitsIO: 6 lines; 227 characters; 12 parameters

void sfitsioExample() {
  fitscc fits;
  fits.append_image("Primary", 0, FITS::SHORT_T, width, height);
  fits_image& primary = fits.image("Primary");
  primary.header(name).assign(value).assign_comment(comment);
  primary.put_data(data, width * height);
  fits.write_stream(filename);
}

// AFW: 4 lines; 185 characters; 11 parameters

void afwExample() {
  Fits fits(filename, "w", AUTO_CLOSE);
  fits.createImage(width, height);
  fits.writeKey(name, value, comment);
  fits.writeImage(external(data, makeVector(double(width), double(height)), makeVector(1, 1))));
}

// EL_FitsIO: 3 lines; 142 characters; 9 parameters

void elfitsioExample() {
  SifFile fits(filename, FileMode::Create);
  fits.header().writeRecord(name, value, "", comment);
  fits.writeRaster(PtrRaster<short>({ width, height }, data));
}
