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
    options.add_options()
        ("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsFile_Example");

    const std::string filename = args["output"].as<std::string>();

    logger.info();

    logger.info() << "Creating Fits file: " << filename;
    //! [Create Fits]
    MefFile f(filename, MefFile::Permission::OVERWRITE);
    //! [Create Fits]
    const auto& primary = f.access_primary<>(); // We don't need to specify the HDU type for metadata work
    logger.info() << "Writing new record: VALUE = 1";
    //! [Write record]
    primary.write_record("VALUE", 1);
    //! [Write record]
    logger.info() << "Updating record: VALUE = 2";
    //! [Update record]
    primary.update_record("VALUE", 2);
    //! [Update record]

    logger.info();

    Test::SmallTable table; // Predefined table for testing purpose
    logger.info() << "Creating bintable extension: SMALLTBL";
    //! [Create bintable ext]
    f.assign_bintable_ext("SMALLTBL", table.num_col, table.radec_col, table.name_col, table.dist_mag_col);
    //! [Create bintable ext]

    logger.info();

    Test::SmallRaster raster; // Predefined image raster for testing purpose
    logger.info() << "Creating image extension: SMALLIMG";
    //! [Create image ext]
    const auto& ext = f.assign_image_ext("SMALLIMG", raster);
    //! [Create image ext]
    logger.info() << "Writing record: STRING = string";
    Record<std::string> str_record("STRING", "string");
    logger.info() << "Writing record: INTEGER = 8";
    Record<int> int_record("INTEGER", 8);
    ext.write_records(str_record, int_record);

    logger.info();

    logger.info() << "Closing file.";
    //! [Close Fits]
    f.close(); // We close the file manually for demo purpose, but this is done by the destructor otherwise
    //! [Close Fits]

    logger.info();

    logger.info() << "Reopening file.";
    //! [Open Fits]
    f.open(filename, MefFile::Permission::READ);
    //! [Open Fits]
    //! [Read record]
    const auto record_value = f.access_primary<>().parse_record<int>("VALUE");
    //! [Read record]
    logger.info() << "Reading record: VALUE = " << record_value;

    logger.info();

    logger.info() << "Reading bintable.";
    //! [Find HDU by name]
    const auto& bintable_ext = f.access_first<BintableHdu>("SMALLTBL");
    //! [Find HDU by name]
    //! [Get HDU index]
    const auto index = bintable_ext.index();
    //! [Get HDU index]
    logger.info() << "HDU index: " << index;
    //! [Read column]
    const auto ids = bintable_ext.read_column<int>("ID").vector();
    const auto first_cell = ids[0];
    //! [Read column]
    logger.info() << "First id: " << first_cell;
    const auto names = bintable_ext.read_column<std::string>("NAME").vector();
    logger.info() << "Last name: " << names[names.size()-1];

    logger.info();
    
    logger.info() << "Reading image.";
    //! [Find HDU by index]
    const auto& ext_3 = f.access<>(3);
    //! [Find HDU by index]
    //! [Get HDU name]
    const auto extname = ext_3.name();
    //! [Get HDU name]
    logger.info() << "Name of HDU #3: " << extname;
    const auto records = ext_3.parse_records<std::string, int>({"STRING", "INTEGER"});
    logger.info() << "Reading record: STRING = " << std::get<0>(records).value;
    logger.info() << "Reading record: INTEGER = " << std::get<1>(records).value;
    const auto& image_ext = f.access_first<ImageHdu>("SMALLIMG");
    //! [Read raster]
    const auto image = image_ext.read_raster<float>();
    const auto first_pixel = image[{0, 0}];
    const auto width = image.length<0>();
    const auto height = image.length<1>();
    const auto last_pixel = image[{width-1, height-1}];
    //! [Read raster]
    logger.info() << "First pixel: " << first_pixel;
    logger.info() << "Last pixel: " << last_pixel;

    logger.info();

    logger.info() << "File will be closed at execution end.";

    logger.info();

    return Elements::ExitCode::OK;
  } // File is closed by destructor

};

MAIN_FOR(EL_FitsFile_Example)
