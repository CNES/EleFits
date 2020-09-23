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

#include "EL_FitsData/FitsDataFixture.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_FitsFile/MefFile.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid;
using namespace FitsIO;

class EL_FitsFile_Example : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    add("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsFile_Example");

    const std::string filename = args["output"].as<std::string>();

    logger.info();

    logger.info() << "Creating Fits file: " << filename;
    //! [Create Fits]
    MefFile f(filename, MefFile::Permission::Overwrite);
    //! [Create Fits]
    const auto &primary = f.accessPrimary<>(); // We don't need to specify the HDU type for metadata work
    logger.info() << "Writing new record: VALUE = 1";
    //! [Write record]
    primary.writeRecord("VALUE", 1);
    //! [Write record]
    logger.info() << "Updating record: VALUE = 2";
    //! [Update record]
    primary.updateRecord("VALUE", 2);
    //! [Update record]

    logger.info();

    Test::SmallTable table; // Predefined table for testing purpose
    logger.info() << "Creating bintable extension: SMALLTBL";
    //! [Create bintable ext]
    f.assignBintableExt("SMALLTBL", table.numCol, table.radecCol, table.nameCol, table.distMagCol);
    //! [Create bintable ext]

    logger.info();

    Test::SmallRaster raster; // Predefined image raster for testing purpose
    logger.info() << "Creating image extension: SMALLIMG";
    //! [Create image ext]
    const auto &ext = f.assignImageExt("SMALLIMG", raster);
    //! [Create image ext]
    logger.info() << "Writing record: STRING = string";
    Record<std::string> strRecord("STRING", "string");
    logger.info() << "Writing record: INTEGER = 8";
    Record<int> intRecord("INTEGER", 8);
    ext.writeRecords(strRecord, intRecord);

    logger.info();

    logger.info() << "Closing file.";
    //! [Close Fits]
    f.close(); // We close the file manually for demo purpose, but this is done by the destructor otherwise
    //! [Close Fits]

    logger.info();

    logger.info() << "Reopening file.";
    //! [Open Fits]
    f.open(filename, MefFile::Permission::Read);
    //! [Open Fits]
    //! [Read record]
    const auto recordValue = f.accessPrimary<>().parseRecord<int>("VALUE");
    //! [Read record]
    logger.info() << "Reading record: VALUE = " << recordValue;

    logger.info();

    logger.info() << "Reading bintable.";
    //! [Find HDU by name]
    const auto &bintableExt = f.accessFirst<BintableHdu>("SMALLTBL");
    //! [Find HDU by name]
    //! [Get HDU index]
    const auto index = bintableExt.index();
    //! [Get HDU index]
    logger.info() << "HDU index: " << index;
    //! [Read column]
    const auto ids = bintableExt.readColumn<int>("ID").vector();
    const auto firstCell = ids[0];
    //! [Read column]
    logger.info() << "First id: " << firstCell;
    const auto names = bintableExt.readColumn<std::string>("NAME").vector();
    logger.info() << "Last name: " << names[names.size() - 1];

    logger.info();

    logger.info() << "Reading image.";
    //! [Find HDU by index]
    const auto &ext3 = f.access<>(3);
    //! [Find HDU by index]
    //! [Get HDU name]
    const auto extname = ext3.readName();
    //! [Get HDU name]
    logger.info() << "Name of HDU #3: " << extname;
    const auto records = ext3.parseRecords<std::string, int>({ "STRING", "INTEGER" });
    logger.info() << "Reading record: STRING = " << std::get<0>(records).value;
    logger.info() << "Reading record: INTEGER = " << std::get<1>(records).value;
    const auto &imageExt = f.accessFirst<ImageHdu>("SMALLIMG");
    //! [Read raster]
    const auto image = imageExt.readRaster<float>();
    const auto firstPixel = image[{ 0, 0 }];
    const auto width = image.length<0>();
    const auto height = image.length<1>();
    const auto lastPixel = image[{ width - 1, height - 1 }];
    //! [Read raster]
    logger.info() << "First pixel: " << firstPixel;
    logger.info() << "Last pixel: " << lastPixel;

    logger.info();

    logger.info() << "File will be closed at execution end.";

    logger.info();

    return Elements::ExitCode::OK;
  } // File is closed by destructor
};

MAIN_FOR(EL_FitsFile_Example)
