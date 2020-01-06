/**
 * @file src/program/EL_FitsIO_GenerateAstroObj.cpp
 * @date 01/06/20
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
#include "EL_FitsFile/MefFile.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid;
using namespace FitsIO;


static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_GenerateAstroObj");

void writeMeta(MefFile& f, int obj_index) {
	std::string extname = std::to_string(obj_index) + "_META";
	const auto& ext = f.init_image_ext<unsigned char, 1>(extname, {0});
	ext.write_record("DITH_NUM", 0); //TODO
	ext.write_record("SOURC_ID", obj_index);
	ext.write_record("RA_OBJ", 2.F * obj_index);
	ext.write_record("DEC_OBJ", 3.F * obj_index);
}

void writeCombined(MefFile& f, int obj_index) {
	const int size(12); //TODO
	std::vector<float> wmin_data(size);
	std::vector<float> signal_data(size);
	std::vector<short> quality_data(size);
	std::vector<float> var_data(size);
	Column<float> wmin_col { "WMIN", 1, "nm", wmin_data };
	Column<float> signal_col { "SIGNAL", 1, "erg", signal_data };
	Column<short> quality_col { "QUALITY", 1, "", quality_data };
	Column<float> var_col { "VAR", 1, "erg^2", var_data };
	std::string extname = std::to_string(obj_index) + "_COMBINED1D_SIGNAL";
	f.assign_bintable_ext(extname, wmin_col, signal_col, quality_col, var_col);
	//TODO cov
}

void writeAstroObj(MefFile& f, int obj_index) {
	writeMeta(f, obj_index);
	writeCombined(f, obj_index);
}

class EL_FitsIO_GenerateAstroObj : public Elements::Program {

public:

	options_description defineSpecificProgramOptions() override {
  
		options_description options {};
		options.add_options()
				("output", value<std::string>()->default_value("/tmp/astroobj.fits"), "Output file")
				("nobj", value<int>()->default_value(1), "AstroObj count");
		return options;
	}

	Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

		Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_GenerateAstroObj");

		std::string filename = args["output"].as<std::string>();
		int nobj = args["nobj"].as<int>();

		logger.info() << "Creating Fits file: " << filename;
		MefFile f(filename, FitsFile::Permission::OVERWRITE);
		logger.info() << "Writing metadata";
		const auto& primary = f.access_primary<>();
		primary.write_record("N_OBJ", nobj);

		for(int i=0; i<nobj; ++i) {
			logger.info() << "Writing AstroObj " << i;
			writeAstroObj(f, i);
		}
		return Elements::ExitCode::OK;
	}

};

MAIN_FOR(EL_FitsIO_GenerateAstroObj)
