// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/LoopingBenchmark.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "Linx/Run/ProgramOptions.h"

#include <map>
#include <string>

using namespace Fits;

Validation::LoopingBenchmark::Duration loop(Validation::LoopingBenchmark& benchmark, char setup)
{
  switch (setup) {
    case 'x':
      return benchmark.loop_over_xyz();
    case 'z':
      return benchmark.loop_over_zyx();
    case 'p':
      return benchmark.loop_over_positions();
    case 'i':
      return benchmark.loop_over_indices();
    case 'v':
      return benchmark.loop_over_values();
    default:
      throw std::runtime_error("Invalid setup"); // FIXME TestNotImplemented
  }
}

int main(int argc, char const* argv[])
{
  Linx::ProgramOptions options;
  options.named<char>("setup", "Test setup to be benchmarked (x, z, p, i, v)");
  options.named<Linx::Index>("side", "Image width, height and depth (same value)", 400);
  options.parse(argc, argv);

  Elements::Logging logger = Elements::Logging::getLogger("EleFitsRunLoopingBenchmark");

  logger.info("Generating random rasters...");
  Validation::LoopingBenchmark benchmark(options.as<Linx::Index>("side"));

  logger.info("Looping over them...");
  const auto duration = loop(benchmark, options.as<char>("setup"));

  logger.info() << "Done in " << duration.count() << "ms";

  return 0;
}
