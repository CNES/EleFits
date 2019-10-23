/**
 * @file src/program/EL_Cfitsio_Example.cpp
 * @date 10/22/19
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

#include <complex>
#include <map>
#include <string>

#include <boost/program_options.hpp>
#include <cfitsio/fitsio.h>

#include "ElementsKernel/ProgramHeaders.h"

#include "EL_CfitsioWrapper/ErrorWrapper.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

static Elements::Logging logger = Elements::Logging::getLogger("EL_Cfitsio_Example");


struct SmallTable {
	static constexpr long cols = 4;
	static constexpr long rows = 3;
	char *col_name[cols] = { "ID", "RADEC", "NAME", "DIST_MAG" };
	char *col_format[cols] = { "1J", "1C", "68A", "2D" };
	char *col_unit[cols] = { nullptr, "deg", nullptr, "kal" };
	int ids[rows] = { 45, 7, 31 };
	std::complex<float> radecs[rows] = { { 56.8500, 24.1167 }, { 268.4667, -34.7928 }, { 10.6833, 41.2692 } };
	char *names[rows] = { "Pleiades", "Ptolemy Cluster", "Andromeda Galaxy" };
	std::vector<double> dist_mags[rows] = { { 0.44, 1.6 }, { 0.8, 3.3 }, { 2900., 3.4 } };
};


class EL_Cfitsio_Example : public Elements::Program {

public:

	options_description defineSpecificProgramOptions() override {
		options_description options {};
			options.add_options()
				("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
		return options;
	}

	Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    	Elements::Logging logger = Elements::Logging::getLogger("EL_Cfitsio_Example");

		const std::string filename = args["output"].as<std::string>();

		logger.info();

		logger.info() << "Creating Fits file: " << filename;
		int status = 0;
		fitsfile *fptr;
		fits_create_file(&fptr, (std::string("!") + filename).c_str(), &status);
		long naxis0 = 0;
		fits_create_img(fptr, BYTE_IMG, 1, &naxis0, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status, "while creating file");

		logger.info() << "Writing new record: VALUE = 1";
		int record_value = 1;
		fits_write_key(fptr, TINT, "VALUE", &record_value, nullptr, &status);
		logger.info() << "Updating record: VALUE = 2";
		record_value = 2;
		fits_update_key(fptr, TINT, "VALUE", &record_value, nullptr, &status);

		logger.info() << "Creating bintable extension: SMALLTBL";
		SmallTable table;
		fits_create_tbl(fptr, BINARY_TBL, 0, table.cols, table.col_name, table.col_format, table.col_unit, "SMALLTBL", &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status, "while creating bintable extension");
		fits_write_col(fptr, TINT, 1, 1, 1, 1, table.ids, &status);
		fits_write_col(fptr, TCOMPLEX, 2, 1, 1, 1, table.radecs, &status);
		fits_write_col(fptr, TSTRING, 3, 1, 1, 1, table.names, &status);
		fits_write_col(fptr, TDOUBLE, 4, 1, 1, 2, table.dist_mags, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status, "while writing columns");

		logger.info() << "Creating image extension: SMALLIMG";
		long naxes[] = { 3, 2 };
		fits_create_img(fptr, FLOAT_IMG, 2, naxes, &status);
		char *extname = "SMALLIMG";
		fits_write_key(fptr, TSTRING, "EXTNAME", extname, nullptr, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status, "while creating image extension");
		float data[] = { 0.0, 0.1, 1.0, 1.1, 2.0, 2.1 };
		fits_write_img(fptr, FLOAT_IMG, 1, 6, data, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status, "while writing raster");

		char *record_string = "string";
		int record_integer = 8;
		fits_write_key(fptr, TSTRING, "STRING", &record_string, nullptr, &status);
		fits_write_key(fptr, TINT, "INTEGER", &record_integer, nullptr, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status);

		logger.info() << "Closing file.";
		fits_close_file(fptr, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status);

		logger.info();

		logger.info() << "Reopening file.";
		fits_open_file(&fptr, filename.c_str(), READONLY, &status);
		fits_read_key(fptr, TINT, "VALUE", &record_value, nullptr, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status);
		logger.info() << "Reading record: VALUE = " << record_value;

		logger.info();

		logger.info() << "Reading bintable.";
		fits_movnam_hdu(fptr, ANY_HDU, "SMALLTBL", 0, &status);
		Euclid::Cfitsio::may_throw_cfitsio_error(status);
		int index;
		fits_get_hdu_num(fptr, &index);
		logger.info() << "HDU index = " << index;
		int ids[3];
		int colnum;
		fits_get_colnum(fptr, CASESEN, "ID", &colnum, &status);
		fits_read_col(fptr, TINT, colnum, 1, 1, 1, nullptr, ids, nullptr, &status);
		logger.info() << "First id: " << ids[0];
		fits_get_colnum(fptr, CASESEN, "NAME", &colnum, &status);
		char names[3][68];
		fits_read_col(fptr, TSTRING, colnum, 1, 1, 68, nullptr, names, nullptr, &status);
		logger.info() << "Last name: " << names[2];

		// logger.info();
		
		// logger.info() << "Reading image.";
		// Hdu::goto_index(fptr, 3);
		// logger.info() << "Name of HDU #3: " << Hdu::current_name(fptr);
		// const auto records = Header::parse_records<std::string, int>(fptr, {"STRING", "INTEGER"});
		// logger.info() << "SMALLIMG.STRING = " << std::get<0>(records).value;
		// logger.info() << "SMALLIMG.INTEGER = " << std::get<1>(records).value;
		// Hdu::goto_name(fptr, "SMALLIMG");
		// const auto image = Image::read_raster<float>(fptr);
		// logger.info() << "First pixel: " << image[{0, 0}];
		// const auto width = image.length<0>();
		// const auto height = image.length<1>();
		// logger.info() << "Last pixel: " << image[{width-1, height-1}];
		// logger.info() << "Reclosing file.";
		// File::close(fptr);

		logger.info();

		return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_Cfitsio_Example)

