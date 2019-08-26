/**
 * @file src/program/EL_FitsIO_Example.cpp
 * @date 07/24/19
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

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/RecordWrapper.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Cfitsio;


static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Example");

class EL_FitsIO_Example : public Elements::Program {

public:

    options_description defineSpecificProgramOptions() override {
    
        options_description options {};
        options.add_options()
            ("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
        return options;
    }

    Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

        Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Example");

        const std::string filename = args["output"].as<std::string>();

        auto fptr = File::create_and_open(filename, File::CreatePolicy::OVER_WRITE);
        Record::write_value(fptr, "VALUE", 1);
        Test::SmallTable table;
        HDU::create_bintable_extension(fptr, "SMALLTBL", table.id_col, table.radec_col, table.name_col, table.dist_mag_col);
        Test::SmallRaster raster;
        HDU::create_image_extension(fptr, "SMALLIMG", raster);
        File::close(fptr);

        fptr = File::open(filename, File::OpenPolicy::READ_WRITE);
        logger.info() << "Value: " << Record::parse_value<int>(fptr, "VALUE");
        Record::update_value(fptr, "VALUE", 2);
        logger.info() << "New value: " << Record::parse_value<int>(fptr, "VALUE");
        HDU::goto_name(fptr, "SMALLTBL");
        logger.info() << "First id: " << Bintable::read_column<int>(fptr, "ID").data[0];
        HDU::goto_name(fptr, "SMALLIMG");
        logger.info() << "Some pixel: " << Image::read_raster<int>(fptr)[{0, 1}];
        File::close(fptr);

        return Elements::ExitCode::OK;

    }

};

MAIN_FOR(EL_FitsIO_Example)



