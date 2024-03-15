// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsExamples/GameOfLife.h"
#include "Linx/Run/ProgramOptions.h"

#include <string>

int main(int argc, char const* argv[])
{
  Linx::ProgramOptions options("Generate and play a Game of Life.");
  options.positional<std::string>("output", "Output file", "/tmp/life.fits");
  options.named<Linx::Index>("width", "Board width", 36);
  options.named<Linx::Index>("height", "Board height", 24);
  options.named<Linx::Index>("turns", "Number of turns", 100);
  options.named<Linx::Index>("init", "Initial number of lives", 200);
  options.parse(argc, argv);

  std::cout << "Generating lives..." << std::endl;
  Fits::GameOfLife gol(
      options.as<Linx::Index>("width"),
      options.as<Linx::Index>("height"),
      options.as<Linx::Index>("turns"));
  const auto card = options.as<Linx::Index>("init");
  const auto& positions = gol.generate(card);

  std::cout << "Playing..." << std::endl;
  const auto& board = gol.run();

  std::cout << "Saving board..." << std::endl;
  Fits::MefFile f(options.as<std::string>("output"), Fits::FileMode::Overwrite);
  const auto& du = f.primary().raster();
  du.update(board);

  std::cout << "Saving initial positions..." << std::endl;
  const auto x_col = Fits::make_column(Fits::make_column_info<Linx::Index>("X"), card, positions.row(0).data());
  const auto y_col = Fits::make_column(Fits::make_column_info<Linx::Index>("Y"), card, positions.row(1).data());
  f.append_bintable("INIT", {}, x_col, y_col);

  std::cout << "Done." << std::endl;
  return 0;
}
