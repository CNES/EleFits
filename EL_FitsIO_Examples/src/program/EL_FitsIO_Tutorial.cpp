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

Raster<float, 3> create_raster() {
  //! [Create and fill a raster]
  int width = 16, height = 9, depth = 3;
  Raster<float, 3> raster({width, height, depth});
  for(int z=0; z<depth; ++z)
    for(int y=0; y<height; ++y)
      for(int x=0; x<width; ++x)
        raster[{x, y, z}] = x + y + z;
  //! [Create and fill a raster]
  return raster;
}

struct TutoTable {
  DataColumn<std::string> name_col;
  DataColumn<double> speed_col;
};

TutoTable create_columns() {
  //! [Create and fill a column]
  std::vector<std::string> name_data { "snail", "Antoine", "light", "Millennium Falcon" };
  std::vector<double> speed_data { 1.3e-2, 1.4, 3.0e8, 4.5e8 };
  TutoTable table {
      DataColumn<std::string>({"NAME", "", 42}, std::move(name_data)),
      DataColumn<double>({"SPEED", "m/s", 1}, std::move(speed_data))
  };
  //! [Create and fill a column]
  return table;
}

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
    //! [Create a complete record]
    Record<float> complete_record("SPEED", 2.5, "m/s", "Already fast!");
    //! [Create a complete record]
    primary.write_record(complete_record);

    //! [Assign a bintable extension]
    const auto columns = create_columns();
    f.assign_bintable_ext("TABLE", columns.name_col, columns.speed_col);
    //! [Assign a bintable extension]

    //! [Assign an image extension]
    const auto raster = create_raster();
    const auto& ext = f.assign_image_ext("IMAGE", raster);
    //! [Assign an image extension]

    //! [Write several records]
    const Record<std::string> str_record("STRING", "string");
    const Record<int> int_record("INTEGER", 8);
    ext.write_records(str_record, int_record);
    //! [Write several records]

    //! [Read a record]
    const auto record = f.access_primary<>().parse_record<int>("VALUE");
    //! [Read a record]

    //! [Access an HDU by name]
    const auto& bintable_ext = f.access_first<BintableHdu>("TABLE");
    //! [Access an HDU by name]
    //! [Read bintable values]
    const auto names = bintable_ext.read_column<std::string>("NAME").data();
    const auto speeds = bintable_ext.read_column<double>("SPEED").data();
    const auto slowest_guy = names[0];
    const auto max_speed = speeds[speeds.size()-1];
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
    const auto image = image_ext.read_raster<float, 3>();
    const auto& first_pixel = image[{0, 0}];
    const auto width = image.length<0>();
    const auto height = image.length<1>();
    const auto depth = image.length<2>();
    const auto& last_pixel = image[{width-1, height-1, depth-1}];
    //! [Read image values]

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_Tutorial)



