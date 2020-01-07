/**
 * @file src/program/EL_FitsIO_Tutorial.cpp
 * @date 01/07/20
 * @author user
 *
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

//! [Include]
#include "EL_FitsFile/MefFile.h"
using namespace Euclid::FitsIO;
//! [Include]

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Tutorial");

class EL_FitsIO_Tutorial : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
  
    options_description options {};
    options.add_options()
        ("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Tutorial");

    const std::string filename = args["output"].as<std::string>();

    //! [Open a MefFile]
    MefFile f(filename, FitsFile::Permission::OVERWRITE);
    //! [Open a MefFile]
    //! [Access primary HDU]
    const auto& primary = f.access_primary<>();
    //! [Access primary HDU]
    //! [Write and update a record]
    primary.write_record("VALUE", 1);
    primary.update_record("VALUE", 2);
    //! [Write and update a record]
    //! [Write a record with comment and unit]
    Record<float> complete_record("COMPLETE", 3.0, "m", "Some comment");
    primary.write_record(complete_record);
    //! [Write a record with comment and unit]

    //! [Assign a bintable extension]
    Test::SmallTable table; // Predefined table for testing purpose
    f.assign_bintable_ext("SMALLTBL", table.num_col, table.radec_col, table.name_col, table.dist_mag_col);
    //! [Assign a bintable extension]

    //! [Assign a raster extension]
    Test::SmallRaster raster; // Predefined image raster for testing purpose
    const auto& ext = f.assign_image_ext("SMALLIMG", raster);
    //! [Assign a raster extension]

    //! [Write several records]
    Record<std::string> str_record("STRING", "string");
    Record<int> int_record("INTEGER", 8);
    ext.write_records(str_record, int_record);
    //! [Write several records]

    //! [Read a record]
    auto record = f.access_primary<>().parse_record<int>("VALUE");
    //! [Read a record]

    //! [Access an HDU by name]
    const auto& bintable_ext = f.access_first<BintableHdu>("SMALLTBL");
    //! [Access an HDU by name]
    //! [Read bintable values]
    const auto nums = bintable_ext.read_column<int>("ID").data;
    const auto first_id = nums[0];
    const auto names = bintable_ext.read_column<std::string>("NAME").data;
    const auto last_name = names[names.size()-1];
    //! [Read bintable values]

    //! [Access an HDU by index]
    const auto& image_ext = f.access<ImageHdu>(3);
    //! [Access an HDU by index]
    //! [Read several records]
    auto records = image_ext.parse_records<std::string, int>({"STRING", "INTEGER"});
    const auto str_value = std::get<0>(records).value;
    const auto int_value = std::get<1>(records).value;
    //! [Read several records]
    //! [Read image values]
    const auto image = image_ext.read_raster<float>();
    const auto first_pixel = image[{0, 0}];
    const auto width = image.length<0>();
    const auto height = image.length<1>();
    const auto last_pixel = image[{width-1, height-1}];
    //! [Read image values]

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_Tutorial)



