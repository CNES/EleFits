/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid::Fits;

/*
 * Some properties of an `AstroObj`.
 */
struct AstroObjInfo {
  long combSize;
  long dithCount;
  long dith1Size;
  Position<2> dith2Shape;
};

/*
 * Write records in the Primary.
 */
void writePrimaryHeader(const Header& h, long nobj) {
  Record<std::int64_t> nobjRecord {"N_OBJ", nobj, "", "number of objects in the package"};
  Record<std::string> telescopeRecord {"TELESCOP", "EUCLID", "", "telescope name"};
  Record<std::string> instrumentRecord {"INSTRUME", "NISP", "", "instrument name"};
  h.writeSeq(nobjRecord, telescopeRecord, instrumentRecord);
}

/*
 * Write a binary table extension which represents one of several `AstroObj`s.
 */
const BintableHdu& writeExt(MefFile& f, const std::string& name, const AstroObjInfo& info, long rowCount) {

  /* Metadata */

  ColumnInfo<std::int64_t> objIdInfo("OBJECT_ID");
  ColumnInfo<std::complex<float>> objRadecInfo("RA_DEC", "deg");
  ColumnInfo<float> expTimeInfo("EXPTIME", "s");

  /* Combined infos */

  const auto combSize = info.combSize;

  ColumnInfo<float> combSigInfo("COMBINED1D_SIGNAL", "erg", combSize);
  ColumnInfo<float> combVarInfo("COMBINED1D_VAR", "erg^2", combSize);

  /* Dithers infos */

  const auto dithCount = info.dithCount;
  const auto dith1Size = info.dith1Size;
  const auto dith2Width = info.dith2Shape[0];
  const auto dith2Height = info.dith2Shape[1];

  ColumnInfo<std::int64_t> ptgIdInfo("PTGID", "", dithCount);

  ColumnInfo<float, 2> dith1SigInfo("DITH1D_SIGNAL", "erg", {dith1Size, dithCount});
  ColumnInfo<float, 2> dith1VarInfo("DITH1D_VAR", "erg^2", {dith1Size, dithCount});

  ColumnInfo<float, 3> dith2SigInfo("DITH2D_SIGNAL", "erg", {dith2Width, dith2Height, dithCount});
  ColumnInfo<float, 3> dith2VarInfo("DITH2D_VAR", "erg^2", {dith2Width, dith2Height, dithCount});

  /* Random data */

  auto objIdData = Test::generateRandomVector<std::int64_t>(rowCount);
  auto objRadecData = Test::generateRandomVector<std::complex<float>>(rowCount);
  auto expTimeData = Test::generateRandomVector<float>(rowCount);

  auto combSigData = Test::generateRandomVector<float>(combSize * rowCount);
  auto combVarData = Test::generateRandomVector<float>(combSize * rowCount);

  auto ptgIdData = Test::generateRandomVector<std::int64_t>(dithCount * rowCount);

  auto dith1SigData = Test::generateRandomVector<float>(dith1Size * dithCount * rowCount);
  auto dith1VarData = Test::generateRandomVector<float>(dith1Size * dithCount * rowCount);

  auto dith2SigData = Test::generateRandomVector<float>(dith2Width * dith2Height * dithCount * rowCount);
  auto dith2VarData = Test::generateRandomVector<float>(dith2Width * dith2Height * dithCount * rowCount);

  /* Create and assign extension */

  return f.assignBintableExt(
      name,
      makeColumn(std::move(objIdInfo), std::move(objIdData)),
      makeColumn(std::move(objRadecInfo), std::move(objRadecData)),
      makeColumn(std::move(expTimeInfo), std::move(expTimeData)),
      makeColumn(std::move(combSigInfo), std::move(combSigData)),
      makeColumn(std::move(combVarInfo), std::move(combVarData)),
      makeColumn(std::move(ptgIdInfo), std::move(ptgIdData)),
      makeColumn(std::move(dith1SigInfo), std::move(dith1SigData)),
      makeColumn(std::move(dith1VarInfo), std::move(dith1VarData)),
      makeColumn(std::move(dith2SigInfo), std::move(dith2SigData)),
      makeColumn(std::move(dith2VarInfo), std::move(dith2VarData)));
}

/**
 * Insert quality columns to a binary table HDU.
 */
void insertColumns(const BintableColumns& du, const AstroObjInfo& info) {

  /* Infos */

  const auto combSize = info.combSize;
  const auto dithCount = info.dithCount;
  const auto dith1Size = info.dith1Size;
  const auto dith2Width = info.dith2Shape[0];
  const auto dith2Height = info.dith2Shape[1];

  ColumnInfo<std::int32_t> combQualInfo("COMBINED1D_QUALITY", "", combSize);
  ColumnInfo<std::int32_t, 2> dith1QualInfo("DITH1D_QUALITY", "", {dith1Size, dithCount});
  ColumnInfo<std::int32_t, 3> dith2QualInfo("DITH2D_QUALITY", "", {dith2Width, dith2Height, dithCount});

  /* Random data */

  const auto rowCount = du.readRowCount();
  auto combQualData = Test::generateRandomVector<std::int32_t>(combSize * rowCount);
  auto dith1QualData = Test::generateRandomVector<std::int32_t>(dith1Size * dithCount * rowCount);
  auto dith2QualData = Test::generateRandomVector<std::int32_t>(dith2Width * dith2Height * dithCount * rowCount);

  /* Insert before variance columns */

  du.init(combQualInfo, du.readIndex("COMBINED1D_VAR"));
  du.init(dith1QualInfo, du.readIndex("DITH1D_VAR"));
  du.init(dith2QualInfo, du.readIndex("DITH2D_VAR"));
  du.writeSeq(
      makeColumn(std::move(combQualInfo), std::move(combQualData)),
      makeColumn(std::move(dith1QualInfo), std::move(dith1QualData)),
      makeColumn(std::move(dith2QualInfo), std::move(dith2QualData)));
}

/*
 * The program.
 */
class EleFitsGenerateAstroObj : public Elements::Program {

public:
  /*
   * Program options.
   */
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options("Generate a random AstroObj file, as specified in the SpectrumLib.");
    options.positional("output", value<std::string>()->default_value("/tmp/astroobj.fits"), "Output file");
    options.named("nobj", value<long>()->default_value(1), "AstroObj count per HDU");
    options.named("nhdu", value<long>()->default_value(1), "HDU count");
    options.named("nbin", value<long>()->default_value(1000), "Wavelength bin count");
    options.named("ndith", value<long>()->default_value(4), "Dither count per AstroObj");
    options.named("height", value<long>()->default_value(15), "Dither 2D height");
    options.flag("qual", "Flag to write quality columns");
    return options.asPair();
  }

  /*
   * Run!
   */
  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateAstroObj");

    std::string filename = args["output"].as<std::string>();
    const auto nobj = args["nobj"].as<long>();
    const auto nhdu = args["nhdu"].as<long>();
    const auto nbin = args["nbin"].as<long>();
    const auto ndith = args["ndith"].as<long>();
    const auto height = args["height"].as<long>();
    const auto qual = args["qual"].as<bool>();

    AstroObjInfo info {nbin, ndith, nbin, {nbin, height}};

    logger.info() << "Creating Fits file: " << filename;
    MefFile f(filename, FileMode::Overwrite);

    logger.info() << "Writing metadata";
    writePrimaryHeader(f.primary().header(), nobj * nhdu);

    for (long i = 0; i < nhdu; ++i) {
      logger.info() << "Writing HDU " << i + 1;
      const auto& ext = writeExt(f, std::to_string(i + 1), info, nobj);
      if (qual) {
        insertColumns(ext.columns(), info);
      }
    }

    logger.info() << "Done.";
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsGenerateAstroObj)
