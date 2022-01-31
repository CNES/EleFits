// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "EleFitsData/Column.h"
#include "EleFitsData/Raster.h"
#include "EleFitsData/Record.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "EleFitsUtils/StringUtils.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Unused.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <map>
#include <string>

using namespace Euclid::Fits;

#define PRINT_SUPPORTED_TYPES(type, name) logger.info() << "  " << #type;

class EleFitsPrintSupportedTypes : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    auto options = ProgramOptions::fromAuxFile("PrintSupportedTypes.txt");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>&) override {
    Elements::Logging logger = Elements::Logging::getLogger("EleFitsPrintSupportedTypes");
    std::string contents = String::readAuxFile("PrintSupportedTypes.txt");
    std::vector<std::string> lines;
    for (const auto& line : boost::split(lines, contents, boost::is_any_of("\n"))) {
      logger.info(line);
    }
    logger.info();
    logger.info("----------------------------");
    logger.info("Supported Record value types");
    logger.info("----------------------------");
    ELEFITS_FOREACH_RECORD_TYPE(PRINT_SUPPORTED_TYPES)
    logger.info("----------------------------");
    logger.info("Supported Raster value types");
    logger.info("----------------------------");
    ELEFITS_FOREACH_RASTER_TYPE(PRINT_SUPPORTED_TYPES)
    logger.info("----------------------------");
    logger.info("Supported Column value types");
    logger.info("----------------------------");
    ELEFITS_FOREACH_COLUMN_TYPE(PRINT_SUPPORTED_TYPES)
    logger.info("----------------------------");
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsPrintSupportedTypes)
