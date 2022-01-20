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
#include "EleFitsData/TestUtils.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

using boost::program_options::value;
using namespace Euclid;

/**
 * Create a binary table extension and write columns of various types and shapes.
 */
const Fits::BintableHdu& writeBintable(Fits::MefFile& f, const std::string& extName, long rowCount) {

  /* A string column */
  auto stringInfo = Fits::makeColumnInfo<std::string>("String", "", 6);
  auto stringData = Fits::Test::generateRandomVector<std::string>(rowCount);
  auto stringCol = makeColumn(stringInfo, std::move(stringData));

  /* A scalar column of complex values */
  auto scalarInfo = Fits::makeColumnInfo<std::complex<float>>("Scalar");
  auto scalarData = Fits::Test::generateRandomVector<std::complex<float>>(rowCount);
  auto scalarCol = makeColumn(scalarInfo, std::move(scalarData));

  /* A vector column of uint32 values */
  auto vectorInfo = Fits::makeColumnInfo<std::uint32_t>("Vector", "", 3);
  auto vectorData = Fits::Test::generateRandomVector<std::uint32_t>(rowCount * vectorInfo.repeatCount());
  auto vectorCol = makeColumn(vectorInfo, std::move(vectorData));

  /* A multidimensional column of int32 values */
  auto multidimInfo = Fits::makeColumnInfo<std::int32_t>("Multidim", "", 3, 2);
  auto multidimData = Fits::Test::generateRandomVector<std::int32_t>(rowCount * multidimInfo.repeatCount());
  auto multidimCol = makeColumn(multidimInfo, std::move(multidimData));

  /* Create the table */
  return f.assignBintableExt(extName, stringCol, scalarCol, vectorCol, multidimCol);
}

class EleFitsBintableExample : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options("Generate, write and read a binary table.");
    options.positional("output", value<std::string>()->default_value("/tmp/bintable.fits"), "Output file");
    options.named("rows", value<long>()->default_value(42), "Number of rows");
    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {
    Logging logger = Logging::getLogger("EleFitsBintableExample");
    Fits::MefFile f(args["output"].as<std::string>(), Fits::FileMode::Write);
    writeBintable(f, "TABLE", args["rows"].as<long>());
    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsBintableExample)
