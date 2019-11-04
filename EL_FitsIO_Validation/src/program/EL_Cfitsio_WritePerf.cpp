/**
 * @file src/program/EL_Cfitsio_WritePerf.cpp
 * @date 11/04/19
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

#include <chrono>
#include <map>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <cfitsio/fitsio.h>

#include "ElementsKernel/ProgramHeaders.h"

//#include "EL_CfitsioWrapper/ErrorWrapper.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

namespace std {
/*
 * Required to get program_options accept vector default values.
 * @see https://stackoverflow.com/questions/40754810
 */
template<typename T>
std::ostream& operator<<(std::ostream &os, const std::vector<T>& vec) {
	for (auto item : vec)
		os << item << " ";
	return os;
}
}

std::vector<float> generate_raster(int naxis1, int naxis2) {
	int order = 10;
	while(order <= naxis2)
		order *= 10;
	std::vector<float> raster(naxis1 * naxis2);
	for(int j=0; j<naxis2; ++j) for(int i=0; i<naxis1; ++i)
		raster[i + j * naxis1] = float(i) + float(j)/order;
	return raster;
}

void create_image_ext(fitsfile* fptr, std::string extname, long* naxes, float* data) {
	int status = 0;
	int nhdu = 0;
	fits_get_num_hdus(fptr, &nhdu, &status);
	fits_create_img(fptr, FLOAT_IMG, 2, naxes, &status);
	fits_write_key(fptr, TSTRING, "EXTNAME", &extname[0], nullptr, &status);
	fits_write_img(fptr, TFLOAT, 1, naxes[0] * naxes[1], data, &status);
}

void create_table_ext(fitsfile* fptr, std::string extname, long* naxes, float* data) {

}

static Elements::Logging logger = Elements::Logging::getLogger("EL_Cfitsio_WritePerf");

class EL_Cfitsio_WritePerf : public Elements::Program {

public:

	options_description defineSpecificProgramOptions() override {
	
		options_description options {};
		options.add_options()
				("images", value<int>()->default_value(0), "Number of image extensions")
				("tables", value<int>()->default_value(0), "Number of bintable extensions")
				("naxis1", value<int>()->default_value(1), "First axis size")
				("naxis2", value<int>()->default_value(1), "Second axis size")
				("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
		return options;
	}

	Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

		Elements::Logging logger = Elements::Logging::getLogger("EL_Cfitsio_WritePerf");

		const auto image_count = args["images"].as<int>();
		const auto table_count = args["tables"].as<int>();
		const auto naxis1 = args["naxis1"].as<int>();
		const auto naxis2 = args["naxis2"].as<int>();
		const auto filename = args["output"].as<std::string>();

		auto raster = generate_raster(naxis1, naxis2); // Not const to avoid copies
		long naxes[] = {naxis1, naxis2};

		logger.info() << "Creating Fits file: " << filename;
		
		int status = 0;
		fitsfile *fptr;
		fits_create_file(&fptr, (std::string("!") + filename).c_str(), &status);
		long naxis0 = 0;
		fits_create_img(fptr, BYTE_IMG, 1, &naxis0, &status);

		
		logger.info() << "Generating " << image_count << " image extension(s)"
				<< " of size " << naxis1 << " x " << naxis2;

		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		for(int i=0; i<image_count; ++i)
			create_image_ext(fptr, "I_" + std::to_string(i), naxes, raster.data());
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		logger.info() << "\tElapsed: " << duration_ms << " ms";

		logger.info() << "Generating " << table_count << " bintable extension(s)"
				<< " of size " << naxis1 << " x " << naxis2;

		begin = std::chrono::steady_clock::now();
		for(int i=0; i<image_count; ++i)
			create_table_ext(fptr, "T_" + std::to_string(i), naxes, raster.data());
		end = std::chrono::steady_clock::now();
		duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		logger.info() << "\tElapsed: " << duration_ms << " ms";

		fits_close_file(fptr, &status);

		return status==0 ? Elements::ExitCode::OK : Elements::ExitCode::NOT_OK;
	}

};

MAIN_FOR(EL_Cfitsio_WritePerf)
