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

#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;
using namespace Fits;

void writeMeta(MefFile& f, int objIndex) {
  std::string extname = std::to_string(objIndex) + "_META";
  const auto& ext = f.initImageExt<unsigned char, 1>(extname, {0});
  ext.header().writeSeq(
      Record<int>("DITH_NUM", 0), // TODO
      Record<int>("SOURC_ID", objIndex),
      Record<float>("RA_OBJ", float(2 * objIndex)),
      Record<float>("DEC_OBJ", float(3 * objIndex)));
}

void writeCombinedSignal(MefFile& f, int objIndex, int bins) {
  auto wminData = Test::generateRandomVector<float>(bins);
  auto signalData = Test::generateRandomVector<float>(bins);
  auto qualityData = Test::generateRandomVector<char>(bins);
  auto varData = Test::generateRandomVector<float>(bins);
  const long repeatCount = 1;
  PtrColumn<float> wminCol({"WMIN", "nm", repeatCount}, bins, wminData.data());
  PtrColumn<float> signalCol({"SIGNAL", "erg", repeatCount}, bins, signalData.data());
  PtrColumn<char> qualityCol({"QUALITY", "", repeatCount}, bins, qualityData.data());
  PtrColumn<float> varCol({"VAR", "erg^2", repeatCount}, bins, varData.data());
  std::string extname = std::to_string(objIndex) + "_COMBINED1D_SIGNAL";
  const auto& ext = f.assignBintableExt(extname, wminCol, signalCol);
  ext.columns().init(qualityCol.info());
  ext.columns().init(varCol.info());
  ext.columns().writeSeq(qualityCol, varCol);
  ext.header().writeSeq(
      Record<float>("WMIN", 0.F),
      Record<float>("BINWIDTH", 1.F),
      Record<int>("BINCOUNT", bins),
      Record<float>("EXPTIME", 3600.F));
}

void writeCombinedCov(MefFile& f, int objIndex, int bins) {
  Test::RandomRaster<float, 2> cov_raster({bins, bins});
  std::string extname = std::to_string(objIndex) + "_COMBINED1D_COV";
  const auto& ext = f.assignImageExt(extname, cov_raster);
  ext.header().writeSeq(Record<int>("COV_SIDE", bins), Record<std::string>("CODEC", "IDENTITY"));
}

void writeCombined(MefFile& f, int objIndex, int bins) {
  writeCombinedSignal(f, objIndex, bins);
  writeCombinedCov(f, objIndex, bins);
}

void writeAstroObj(MefFile& f, int objIndex, int bins) {
  writeMeta(f, objIndex);
  writeCombined(f, objIndex, bins);
}

class EleFitsGenerateAstroObj : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options("Generate a random AstroObj file, as specified in the SpectrumLib.");
    options.positional("output", value<std::string>()->default_value("/tmp/astroobj.fits"), "Output file");
    options.named("nobj", value<int>()->default_value(1), "AstroObj count");
    options.named("nbin", value<int>()->default_value(1000), "Wavelength bin count");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateAstroObj");

    std::string filename = args["output"].as<std::string>();
    int nobj = args["nobj"].as<int>();
    int nbin = args["nbin"].as<int>();

    logger.info() << "Creating Fits file: " << filename;
    MefFile f(filename, FileMode::Overwrite);
    logger.info() << "Writing metadata";
    const auto& primary = f.primary();
    primary.header().write("N_OBJ", nobj);

    for (int i = 0; i < nobj; ++i) {
      logger.info() << "Writing AstroObj " << i;
      writeAstroObj(f, i, nbin);
    }
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsGenerateAstroObj)
