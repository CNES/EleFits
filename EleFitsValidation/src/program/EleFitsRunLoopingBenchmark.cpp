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

#include "EleFitsUtils/ProgramOptions.h"
#include "EleFitsValidation/LoopingBenchmark.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid::Fits;

Validation::LoopingBenchmark::Duration loop(Validation::LoopingBenchmark& benchmark, char setup) {
  switch (setup) {
    case 'x':
      return benchmark.loopOverXyz();
    case 'z':
      return benchmark.loopOverZyx();
    case 'p':
      return benchmark.loopOverPositions();
    case 'i':
      return benchmark.loopOverIndices();
    case 'v':
      return benchmark.loopOverValues();
    default:
      throw std::runtime_error("Invalid setup"); // FIXME TestNotImplemented
  }
}

class EleFitsRunLoopingBenchmark : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    ProgramOptions options;
    options.named("setup", value<char>(), "Test setup to be benchmarked (x, z, p, i, v)");
    options.named("side", value<long>()->default_value(400), "Image width, height and depth (same value)");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsRunLoopingBenchmark");

    logger.info("Generating random rasters...");
    Validation::LoopingBenchmark benchmark(args["side"].as<long>());

    logger.info("Looping over them...");
    const auto duration = loop(benchmark, args["setup"].as<char>());

    logger.info() << "Done in " << duration.count() << "ms";

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsRunLoopingBenchmark)
