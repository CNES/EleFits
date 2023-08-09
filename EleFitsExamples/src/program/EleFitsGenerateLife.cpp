// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    options.named("init", value<long>()->default_value(200), "Initial number of lives");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {
    Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateLife");

    logger.info("Generating lives...");
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
