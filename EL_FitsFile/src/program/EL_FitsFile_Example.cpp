/**
 * @file src/program/EL_FitsFile_Example.cpp
 * @date 09/02/19
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
#include "EL_FitsFile/MefFile.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid::FitsIO;

static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsFile_Example");

class EL_FitsFile_Example : public Elements::Program {

public:

	options_description defineSpecificProgramOptions() override {

		options_description options {};
				("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
		return options;
	}

	Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

	Elements::Logging logger = Elements::Logging::getLogger("EL_FitsFile_Example");

		const std::string filename = args["output"].as<std::string>();

		logger.info();

		logger.info() << "Creating Fits file: " << filename;
		MefFile f(filename, FitsFile::Permission::OVERWRITE);
		logger.info() << "Writing new record: VALUE = 1";
		auto& primary = f.access_primary<>(); // We don't need to specify the HDU type for metadata work
		primary.write_value("VALUE", 1);
		logger.info() << "Updating record: VALUE = 2";
		primary.update_value("VALUE", 2);
		// Test::SmallTable table; // Predefined table for testing purpose
		// logger.info() << "Creating bintable extension: SMALLTBL";
		// HDU::create_bintable_extension(fptr, "SMALLTBL", table.id_col, table.radec_col, table.name_col, table.dist_mag_col);
		Cfitsio::Test::SmallRaster raster; // Predefined image raster for testing purpose
		logger.info() << "Creating image extension: SMALLIMG";
		f.append<ImageHdu>("SMALLIMG").write_raster(raster);
		logger.info() << "Closing file.";
		f.close(); // We close the file manually for demo purpose, but this is done by the destructor otherwise

		logger.info();

		logger.info() << "Reopening file.";
		f.open(filename, FitsFile::Permission::READ);
		logger.info() << "Reading record: VALUE = " << f.access_primary<>().parse_value<int>("VALUE");

		logger.info();

		logger.info() << "Reading bintable.";
		auto bintable_ext = f.access<BintableHdu>(2);
		const auto ids = bintable_ext.read_column<int>("ID").data;
		logger.info() << "First id: " << ids[0];
		const auto names = bintable_ext.read_column<std::string>("NAME").data;
		logger.info() << "Last name: " << names[names.size()-1];

		logger.info();
		
		logger.info() << "Reading image.";
		auto image_ext = f.access_first<ImageHdu>("SMALLIMG");
		const auto image = image_ext.read_raster<float>();
		logger.info() << "First pixel: " << image[{0, 0}];
		const auto width = image.length<0>();
		const auto height = image.length<1>();
		logger.info() << "Last pixel: " << image[{width-1, height-1}];

		logger.info();

		return Elements::ExitCode::OK;
	} // File is closed by destructor

};

MAIN_FOR(EL_FitsFile_Example)



