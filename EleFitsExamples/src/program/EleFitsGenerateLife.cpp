// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/SifFile.h"
#include "EleFitsExamples/GameOfLife.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "Linx/Run/ProgramOptions.h"

#include <map>
#include <string>

using namespace Fits;

int main(int argc, char const* argv[])
{
  Linx::ProgramOptions options("Generate and play a Game of Life.");
  options.positional<std::string>("output", "Output file", "/tmp/life.fits");
  options.named<Linx::Index>("width", "Board width", 36);
  options.named<Linx::Index>("height", "Board height", 24);
  options.named<Linx::Index>("turns", "Number of turns", 100);
  options.named<Linx::Index>("init", "Initial number of lives", 200);
  options.parse(argc, argv);

  Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateLife");

  logger.info("Generating lives...");
  GameOfLife gol(options.as<Linx::Index>("width"), options.as<Linx::Index>("height"), options.as<Linx::Index>("turns"));
  gol.generate(options.as<Linx::Index>("init"));

  logger.info("Playing...");
  const auto& board = gol.run();

  logger.info("Saving board...");
  SifFile f(options.as<std::string>("output"), FileMode::Overwrite);
  f.write({}, board);

  logger.info("Done.");
  return 0;
}
