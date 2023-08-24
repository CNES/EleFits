// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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

class EleFitsRunLoopingBenchmark : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    ProgramOptions options;
    options.named("setup", value<char>(), "Test setup to be benchmarked (x, z, p, i, v)");
    options.named("side", value<long>()->default_value(400), "Image width, height and depth (same value)");
    return options.as_pair();
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
