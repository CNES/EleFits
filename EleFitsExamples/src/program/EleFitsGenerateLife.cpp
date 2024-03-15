// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

//! [Includes]
#include "EleFits/MefFile.h"
#include "EleFits/SifFile.h"
//! [Includes]

#include "EleFitsExamples/GameOfLife.h"
#include "Linx/Run/ProgramOptions.h"

#include <string>

int main(int argc, char const* argv[])
{
  //! [Declare options]
  Linx::ProgramOptions options("Generate and play a Game of Life.");
  options.positional<std::string>("output", "Output file", "/tmp/life.fits");
  options.named<Linx::Index>("width,w", "Board width", 36);
  options.named<Linx::Index>("height,h", "Board height", 24);
  options.named<Linx::Index>("turns,t", "Number of turns", 100);
  options.named<Linx::Index>("init,n", "Initial number of lives", 200);
  options.flag("cat", "Save the list of initial positions");
  //! [Declare options]

  //! [Parse options]
  options.parse(argc, argv);
  const auto filename = options.as<std::string>("output");
  const auto width = options.as<Linx::Index>("width");
  const auto height = options.as<Linx::Index>("height");
  const auto turns = options.as<Linx::Index>("turns");
  const auto seed_count = options.as<Linx::Index>("init");
  //! [Parse options]

  std::cout << "Generating lives..." << std::endl;
  //! [Setup board]
  Fits::GameOfLife gol(width, height, turns);
  const auto& positions = gol.generate(seed_count);
  //! [Setup board]

  std::cout << "Playing..." << std::endl;
  //! [Play game]
  const auto& board = gol.run();
  //! [Play game]

  if (options.has("cat")) {
    std::cout << "Saving board..." << std::endl;
    //! [Create MEF]
    Fits::MefFile f(filename, Fits::FileMode::Overwrite);
    //! [Create MEF]

    //! [Write Primary]
    f.primary().raster().update(board);
    //! [Write Primary]

    std::cout << "Saving initial positions..." << std::endl;
    //! [Create columns]
    const auto x_col = Fits::make_column(Fits::make_column_info<Linx::Index>("X"), seed_count, positions.row(0).data());
    const auto y_col = Fits::make_column(Fits::make_column_info<Linx::Index>("Y"), seed_count, positions.row(1).data());
    //! [Create columns]

    //! [Append Bintable]
    f.append_bintable("INIT", {}, x_col, y_col);
    //! [Append Bintable]
  } else {
    std::cout << "Saving board..." << std::endl;
    //! [Create SIF]
    Fits::SifFile f(filename, Fits::FileMode::Overwrite);
    //! [Create SIF]

    //! [Write SIF]
    f.write({}, board);
    //! [Write SIF]
  }

  std::cout << "Done." << std::endl;
  return 0;
}
