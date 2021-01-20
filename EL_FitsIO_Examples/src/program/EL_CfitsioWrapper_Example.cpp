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

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"

#include "EL_FitsData/TestColumn.h"
#include "EL_FitsData/TestRaster.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioWrapper.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid;
using namespace Cfitsio;

class EL_CfitsioWrapper_Example : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    add("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_CfitsioWrapper_Example");

    const auto filename = args["output"].as<std::string>();

    logger.info();

    logger.info() << "Creating Fits file: " << filename;
    //! [Create Fits]
    auto fptr = File::createAndOpen(filename, File::CreatePolicy::OverWrite);
    //! [Create Fits]
    logger.info() << "Writing new record: VALUE = 1";
    //! [Write record]
    Header::writeRecord<int>(fptr, { "VALUE", 1 });
    //! [Write record]
    logger.info() << "Updating record: VALUE = 2";
    //! [Update record]
    Header::updateRecord<int>(fptr, { "VALUE", 2 });
    //! [Update record]

    logger.info();

    logger.info() << "Creating binary table extension: SMALLTBL";
    FitsIO::Test::SmallTable table; // Predefined table for testing purpose
    //! [Create binary table ext]
    Hdu::createBintableExtension(fptr, "SMALLTBL", table.numCol, table.radecCol, table.nameCol, table.distMagCol);
    //! [Create binary table ext]

    logger.info();

    logger.info() << "Creating image extension: SMALLIMG";
    FitsIO::Test::SmallRaster raster; // Predefined image raster for testing purpose
    //! [Create image ext]
    Hdu::createImageExtension(fptr, "SMALLIMG", raster);
    //! [Create image ext]
    logger.info() << "Writing record: STRING = string";
    FitsIO::Record<std::string> strRecord("STRING", "string");
    logger.info() << "Writing record: INTEGER = 8";
    FitsIO::Record<int> intRecord("INTEGER", 8);
    Header::writeRecords(fptr, strRecord, intRecord);

    logger.info();

    logger.info() << "Closing file.";
    //! [Close Fits]
    File::close(fptr);
    //! [Close Fits]

    logger.info();

    logger.info() << "Reopening file.";
    //! [Open Fits]
    fptr = File::open(filename, File::OpenPolicy::ReadOnly);
    //! [Open Fits]
    //! [Read record]
    const auto recordValue = Header::parseRecord<int>(fptr, "VALUE");
    //! [Read record]
    logger.info() << "Reading record: VALUE = " << recordValue;

    logger.info();

    logger.info() << "Reading binary table.";
    //! [Find HDU by name]
    Hdu::gotoName(fptr, "SMALLTBL");
    //! [Find HDU by name]
    //! [Get HDU index]
    const auto index = Hdu::currentIndex(fptr);
    //! [Get HDU index]
    logger.info() << "HDU index: " << index;
    //! [Read column]
    const auto ids = Bintable::readColumn<int>(fptr, "ID").vector();
    const auto firstCell = ids[0];
    //! [Read column]
    logger.info() << "First id: " << firstCell;
    const auto names = Bintable::readColumn<std::string>(fptr, "NAME").vector();
    logger.info() << "Last name: " << names[names.size() - 1];

    logger.info();

    logger.info() << "Reading image.";
    //! [Find HDU by index]
    Hdu::gotoIndex(fptr, 3);
    //! [Find HDU by index]
    //! [Get HDU name]
    const auto extname = Hdu::currentName(fptr);
    //! [Get HDU name]
    logger.info() << "Name of HDU #3: " << extname;
    const auto records = Header::parseRecords<std::string, int>(fptr, { "STRING", "INTEGER" });
    logger.info() << "Reading record: STRING = " << std::get<0>(records).value;
    logger.info() << "Reading record: INTEGER = " << std::get<1>(records).value;
    Hdu::gotoName(fptr, "SMALLIMG");
    //! [Read raster]
    const auto image = Image::readRaster<float>(fptr);
    const auto firstPixel = image[{ 0, 0 }];
    const auto lastPixel = image.at({ -1, -1 }); // at() allows backward indexing
    //! [Read raster]
    logger.info() << "First pixel: " << firstPixel;
    logger.info() << "Last pixel: " << lastPixel;

    logger.info();

    logger.info() << "Reclosing file.";
    File::close(fptr);

    logger.info();

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_CfitsioWrapper_Example)
