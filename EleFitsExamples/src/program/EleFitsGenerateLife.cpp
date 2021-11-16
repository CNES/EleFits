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

#include "EleFits/SifFile.h"
#include "EleFitsExamples/GameOfLife.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid::Fits;

class EleFitsGenerateLife : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options("Generate and play a Game of Life.");
    options.positional("output", value<std::string>()->default_value("/tmp/life.fits"), "Output file");
    options.named("width", value<long>()->default_value(36), "Board width");
    options.named("height", value<long>()->default_value(24), "Board height");
    options.named("turns", value<long>()->default_value(100), "Number of turns");
    options.named("init", value<long>()->default_value(200), "Initial number of lifes");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateLife");

    logger.info("Generating lifes...");
    GameOfLife gol(args["width"].as<long>(), args["height"].as<long>(), args["turns"].as<long>());
    gol.generate(args["init"].as<long>());

    logger.info("Playing...");
    const auto& board = gol.run();

    logger.info("Saving board...");
    SifFile f(args["output"].as<std::string>(), FileMode::Overwrite);
    f.writeRaster(board);

    logger.info("Done.");
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsGenerateLife)
