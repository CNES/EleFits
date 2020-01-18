/**
 * @file src/program/EL_CfitsioWrapper_Example.cpp
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

#include "EL_FitsData/FitsDataFixture.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioWrapper.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid;
using namespace Cfitsio;


static Elements::Logging logger = Elements::Logging::getLogger("EL_CfitsioWrapper_Example");

class EL_CfitsioWrapper_Example : public Elements::Program {

public:

	options_description defineSpecificProgramOptions() override {
	
		options_description options {};
		options.add_options()
			("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
		return options;
	}

	Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

		Elements::Logging logger = Elements::Logging::getLogger("EL_CfitsioWrapper_Example");

		const std::string filename = args["output"].as<std::string>();

		logger.info();

		logger.info() << "Creating Fits file: " << filename;
		auto fptr = File::create_and_open(filename, File::CreatePolicy::OVER_WRITE);
		logger.info() << "Writing new record: VALUE = 1";
		Header::write_record<int>(fptr, { "VALUE", 1 } );
		logger.info() << "Updating record: VALUE = 2";
		Header::update_record<int>(fptr, { "VALUE", 2 } );
		FitsIO::Test::SmallTable table; // Predefined table for testing purpose

		logger.info();

		logger.info() << "Creating bintable extension: SMALLTBL";
		Hdu::create_bintable_extension(fptr, "SMALLTBL", table.num_col, table.radec_col, table.name_col, table.dist_mag_col);
		FitsIO::Test::SmallRaster raster; // Predefined image raster for testing purpose

		logger.info();

		logger.info() << "Creating image extension: SMALLIMG";
		Hdu::create_image_extension(fptr, "SMALLIMG", raster);
		logger.info() << "Writing record: STRING = string";
		FitsIO::Record<std::string> str_record("STRING", "string");
		logger.info() << "Writing record: INTEGER = 8";
		FitsIO::Record<int> int_record("INTEGER", 8);
		Header::write_records(fptr, str_record, int_record);

		logger.info();

		logger.info() << "Closing file.";
		File::close(fptr);

		logger.info();

		logger.info() << "Reopening file.";
		fptr = File::open(filename, File::OpenPolicy::READ_ONLY);
		logger.info() << "Reading record: VALUE = " << Header::parse_record<int>(fptr, "VALUE");

		logger.info();

		logger.info() << "Reading bintable.";
		Hdu::goto_name(fptr, "SMALLTBL");
		logger.info() << "HDU index: " << Hdu::current_index(fptr);
		const auto nums = Bintable::read_column<int>(fptr, "ID").vector();
		logger.info() << "First id: " << nums[0];
		const auto names = Bintable::read_column<std::string>(fptr, "NAME").vector();
		logger.info() << "Last name: " << names[names.size()-1];

		logger.info();
		
		logger.info() << "Reading image.";
		Hdu::goto_index(fptr, 3);
		logger.info() << "Name of HDU #3: " << Hdu::current_name(fptr);
		const auto records = Header::parse_records<std::string, int>(fptr, {"STRING", "INTEGER"});
		logger.info() << "Reading record: STRING = " << std::get<0>(records).value;
		logger.info() << "Reading record: INTEGER = " << std::get<1>(records).value;
		Hdu::goto_name(fptr, "SMALLIMG");
		const auto image = Image::read_raster<float>(fptr);
		logger.info() << "First pixel: " << image[{0, 0}];
		const auto width = image.length<0>();
		const auto height = image.length<1>();
		logger.info() << "Last pixel: " << image[{width-1, height-1}];

		logger.info();

		logger.info() << "Reclosing file.";
		File::close(fptr);

		logger.info();

		return Elements::ExitCode::OK;
	}

};

MAIN_FOR(EL_CfitsioWrapper_Example)
