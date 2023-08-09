// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "EleFitsUtils/StringUtils.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <complex>
#include <fitsio.h>
#include <map>
#include <string>

using boost::program_options::value;
using namespace Euclid;

struct SmallTable {
  static constexpr long column_count = 4;
  static constexpr long row_count = 3;
  Fits::String::CStrArray column_names {"ID", "RADEC", "NAME", "DIST_MAG"};
  Fits::String::CStrArray column_formats {"1J", "1C", "68A", "2D"};
  Fits::String::CStrArray column_units {"", "deg", "", "kal"};
  int ids[row_count] = {45, 7, 31};
  std::complex<float> radecs[row_count] = {{56.8500F, 24.1167F}, {268.4667F, -34.7928F}, {10.6833F, 41.2692F}};
  Fits::String::CStrArray names {"Pleiades", "Ptolemy Cluster", "Ptolemy Cluster"};
  std::vector<double> dists_mags[row_count] = {{0.44, 1.6}, {0.8, 3.3}, {2900., 3.4}};
};

struct SmallImage {
  static constexpr long naxis1 = 3;
  static constexpr long naxis2 = 2;
  static constexpr long size = naxis1 * naxis2;
  long naxes[2] = {naxis1, naxis2};
  float data[size] = {0.0F, 0.1F, 1.0F, 1.1F, 2.0F, 2.1F};
};

class EleCfitsioExample : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options;
    options.positional("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleCfitsioExample");

    const std::string filename = args["output"].as<std::string>();

    logger.info();

    logger.info() << "Creating FITS file: " << filename;
    int status = 0;
    //! [Create FITS]
    fitsfile* fptr;
    fits_create_file(&fptr, (std::string("!") + filename).c_str(), &status);
    long naxis0 = 0;
    fits_create_img(fptr, BYTE_IMG, 1, &naxis0, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");
    //! [Create FITS]
    logger.info() << "Writing new record: VALUE = 1";
    //! [Write record]
    int record_value = 1;
    fits_write_key(fptr, TINT, "VALUE", &record_value, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write VALUE");
    //! [Write record]
    logger.info() << "Updating record: VALUE = 2";
    //! [Update record]
    record_value = 2;
    fits_update_key(fptr, TINT, "VALUE", &record_value, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot update VALUE");
    //! [Update record]

    logger.info();

    logger.info() << "Creating binary table extension: SMALLTBL";
    SmallTable table;
    //! [Create binary table ext]
    fits_create_tbl(
        fptr,
        BINARY_TBL,
        0,
        table.column_count,
        table.column_names.data(),
        table.column_formats.data(),
        table.column_units.data(),
        "SMALLTBL",
        &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create binary table extension");
    fits_write_col(fptr, TINT, 1, 1, 1, table.row_count, table.ids, &status);
    fits_write_col(fptr, TCOMPLEX, 2, 1, 1, table.row_count, table.radecs, &status);
    fits_write_col(fptr, TSTRING, 3, 1, 1, table.row_count, table.names.data(), &status);
    fits_write_col(fptr, TDOUBLE, 4, 1, 1, table.row_count * 2, table.dists_mags, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write column_count");
    //! [Create binary table ext]

    logger.info();

    logger.info() << "Creating image extension: SMALLIMG";
    //! [Create image ext]
    SmallImage image;
    fits_create_img(fptr, FLOAT_IMG, 2, image.naxes, &status);
    char* extname = const_cast<char*>("SMALLIMG");
    fits_write_key(fptr, TSTRING, "EXTNAME", extname, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create image extension");
    fits_write_img(fptr, TFLOAT, 1, 6, image.data, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write raster");
    //! [Create image ext]
    char* string_record = const_cast<char*>("string");
    int integer_record = 8;
    logger.info() << "Writing record: STRING = string";
    fits_write_key(fptr, TSTRING, "STRING", string_record, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write STRING");
    logger.info() << "Writing record: INTEGER = 8";
    fits_write_key(fptr, TINT, "INTEGER", &integer_record, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write INTEGER");

    logger.info();

    logger.info() << "Closing file.";
    //! [Close FITS]
    fits_close_file(fptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");
    //! [Close FITS]

    logger.info();

    logger.info() << "Reopening file.";
    //! [Open FITS]
    fits_open_file(&fptr, filename.c_str(), READONLY, &status);
    //! [Open FITS]
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot reopen file");
    //! [Read record]
    fits_read_key(fptr, TINT, "VALUE", &record_value, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read VALUE");
    //! [Read record]
    logger.info() << "Reading record: VALUE = " << record_value;

    logger.info();

    logger.info() << "Reading binary table.";
    //! [Find HDU by name]
    fits_movnam_hdu(fptr, ANY_HDU, const_cast<char*>("SMALLTBL"), 0, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot move to binary table extension");
    //! [Find HDU by name]
    //! [Get HDU index]
    int index;
    fits_get_hdu_num(fptr, &index);
    //! [Get HDU index]
    logger.info() << "HDU index: " << index;
    //! [Read column]
    int colnum;
    fits_get_colnum(fptr, CASESEN, const_cast<char*>("ID"), &colnum, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot find column ID");
    int ids[table.row_count];
    fits_read_col(fptr, TINT, colnum, 1, 1, table.row_count, nullptr, ids, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read column ID");
    const auto first_entry = ids[0];
    //! [Read column]
    logger.info() << "First id: " << first_entry;
    fits_get_colnum(fptr, CASESEN, const_cast<char*>("NAME"), &colnum, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot find column NAME");
    char* names[table.row_count];
    for (int i = 0; i < table.row_count; ++i) {
      names[i] = (char*)malloc(68);
    }
    fits_read_col(fptr, TSTRING, colnum, 1, 1, table.row_count, nullptr, names, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read column NAME");
    logger.info() << "Last name: " << names[table.row_count - 1];
    for (int i = 0; i < table.row_count; ++i) {
      free(names[i]);
    }

    logger.info();

    logger.info() << "Reading image.";
    //! [Find HDU by index]
    fits_movabs_hdu(fptr, 3, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot move to image extension");
    //! [Find HDU by index]
    //! [Get HDU name]
    char* extname_read = (char*)malloc(69);
    fits_read_key(fptr, TSTRING, "EXTNAME", extname_read, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read extension name");
    //! [Get HDU name]
    logger.info() << "Name of HDU #3: " << extname_read;
    free(extname_read);
    char* string_read = (char*)malloc(69);
    fits_read_key(fptr, TSTRING, "STRING", string_read, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read STRING record");
    logger.info() << "Reading record: STRING = " << string_read;
    free(string_read);
    int integer_read;
    fits_read_key(fptr, TINT, "INTEGER", &integer_read, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read INTEGER record");
    logger.info() << "Reading record: INTEGER = " << integer_read;
    //! [Read raster]
    float data[image.size];
    fits_read_img(fptr, TFLOAT, 1, image.size, nullptr, data, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read image raster");
    const auto first_pixel = data[0];
    const auto last_pixel = data[image.size - 1];
    //! [Read raster]
    logger.info() << "First pixel: " << first_pixel;
    logger.info() << "Last pixel: " << last_pixel;

    logger.info();

    logger.info() << "Reclosing file.";
    fits_close_file(fptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");

    logger.info();

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleCfitsioExample)
