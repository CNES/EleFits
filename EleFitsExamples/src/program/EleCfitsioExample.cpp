/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "EleCfitsioWrapper/CfitsioUtils.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <complex>
#include <fitsio.h>
#include <map>
#include <string>

using boost::program_options::value;
using Euclid::Cfitsio::CStrArray;

struct SmallTable {
  static constexpr long cols = 4;
  static constexpr long rows = 3;
  CStrArray colName { "ID", "RADEC", "NAME", "DIST_MAG" };
  CStrArray colFormat { "1J", "1C", "68A", "2D" };
  CStrArray colUnit { "", "deg", "", "kal" };
  int ids[rows] = { 45, 7, 31 };
  std::complex<float> radecs[rows] = { { 56.8500F, 24.1167F }, { 268.4667F, -34.7928F }, { 10.6833F, 41.2692F } };
  CStrArray names { "Pleiades", "Ptolemy Cluster", "Ptolemy Cluster" };
  std::vector<double> dist_mags[rows] = { { 0.44, 1.6 }, { 0.8, 3.3 }, { 2900., 3.4 } };
};

struct SmallImage {
  static constexpr long naxis1 = 3;
  static constexpr long naxis2 = 2;
  static constexpr long size = naxis1 * naxis2;
  long naxes[2] = { naxis1, naxis2 };
  float data[size] = { 0.0F, 0.1F, 1.0F, 1.1F, 2.0F, 2.1F };
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

    logger.info() << "Creating Fits file: " << filename;
    int status = 0;
    //! [Create Fits]
    fitsfile* fptr;
    fits_create_file(&fptr, (std::string("!") + filename).c_str(), &status);
    long naxis0 = 0;
    fits_create_img(fptr, BYTE_IMG, 1, &naxis0, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");
    //! [Create Fits]
    logger.info() << "Writing new record: VALUE = 1";
    //! [Write record]
    int recordValue = 1;
    fits_write_key(fptr, TINT, "VALUE", &recordValue, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write VALUE");
    //! [Write record]
    logger.info() << "Updating record: VALUE = 2";
    //! [Update record]
    recordValue = 2;
    fits_update_key(fptr, TINT, "VALUE", &recordValue, nullptr, &status);
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
        table.cols,
        table.colName.data(),
        table.colFormat.data(),
        table.colUnit.data(),
        "SMALLTBL",
        &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create binary table extension");
    fits_write_col(fptr, TINT, 1, 1, 1, table.rows, table.ids, &status);
    fits_write_col(fptr, TCOMPLEX, 2, 1, 1, table.rows, table.radecs, &status);
    fits_write_col(fptr, TSTRING, 3, 1, 1, table.rows, table.names.data(), &status);
    fits_write_col(fptr, TDOUBLE, 4, 1, 1, table.rows * 2, table.dist_mags, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write columns");
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
    char* recordString = const_cast<char*>("string");
    int recordInteger = 8;
    logger.info() << "Writing record: STRING = string";
    fits_write_key(fptr, TSTRING, "STRING", recordString, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write STRING");
    logger.info() << "Writing record: INTEGER = 8";
    fits_write_key(fptr, TINT, "INTEGER", &recordInteger, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot write INTEGER");

    logger.info();

    logger.info() << "Closing file.";
    //! [Close Fits]
    fits_close_file(fptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");
    //! [Close Fits]

    logger.info();

    logger.info() << "Reopening file.";
    //! [Open Fits]
    fits_open_file(&fptr, filename.c_str(), READONLY, &status);
    //! [Open Fits]
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot reopen file");
    //! [Read record]
    fits_read_key(fptr, TINT, "VALUE", &recordValue, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read VALUE");
    //! [Read record]
    logger.info() << "Reading record: VALUE = " << recordValue;

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
    int ids[table.rows];
    fits_read_col(fptr, TINT, colnum, 1, 1, table.rows, nullptr, ids, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read column ID");
    const auto firstCell = ids[0];
    //! [Read column]
    logger.info() << "First id: " << firstCell;
    fits_get_colnum(fptr, CASESEN, const_cast<char*>("NAME"), &colnum, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot find column NAME");
    char* names[table.rows];
    for (int i = 0; i < table.rows; ++i) {
      names[i] = (char*)malloc(68);
    }
    fits_read_col(fptr, TSTRING, colnum, 1, 1, table.rows, nullptr, names, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read column NAME");
    logger.info() << "Last name: " << names[table.rows - 1];
    for (int i = 0; i < table.rows; ++i) {
      free(names[i]);
    }

    logger.info();

    logger.info() << "Reading image.";
    //! [Find HDU by index]
    fits_movabs_hdu(fptr, 3, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot move to image extension");
    //! [Find HDU by index]
    //! [Get HDU name]
    char* extnameRead = (char*)malloc(69);
    fits_read_key(fptr, TSTRING, "EXTNAME", extnameRead, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read extension name");
    //! [Get HDU name]
    logger.info() << "Name of HDU #3: " << extnameRead;
    free(extnameRead);
    char* stringRead = (char*)malloc(69);
    fits_read_key(fptr, TSTRING, "STRING", stringRead, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read STRING record");
    logger.info() << "Reading record: STRING = " << stringRead;
    free(stringRead);
    int integerRead;
    fits_read_key(fptr, TINT, "INTEGER", &integerRead, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read INTEGER record");
    logger.info() << "Reading record: INTEGER = " << integerRead;
    //! [Read raster]
    float data[image.size];
    fits_read_img(fptr, TFLOAT, 1, image.size, nullptr, data, nullptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot read image raster");
    const auto firstPixel = data[0];
    const auto lastPixel = data[image.size - 1];
    //! [Read raster]
    logger.info() << "First pixel: " << firstPixel;
    logger.info() << "Last pixel: " << lastPixel;

    logger.info();

    logger.info() << "Reclosing file.";
    fits_close_file(fptr, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");

    logger.info();

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleCfitsioExample)
