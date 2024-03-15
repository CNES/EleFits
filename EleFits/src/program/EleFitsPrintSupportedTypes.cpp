// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "EleFitsData/Column.h"
#include "EleFitsData/Raster.h"
#include "EleFitsData/Record.h"
#include "EleFitsUtils/StringUtils.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Unused.h"
#include "Linx/Run/ProgramOptions.h"

#include <boost/algorithm/string.hpp>
#include <map>
#include <string>

using namespace Fits;

#define PRINT_SUPPORTED_TYPES(type, name) logger.info() << "  " << #type;

int main(int argc, char const* argv[])
{
  Elements::Logging logger = Elements::Logging::getLogger("EleFitsPrintSupportedTypes");

  Linx::ProgramOptions options; // FIXME desc
  options.parse(argc, argv);

  std::string contents = String::read_aux_file("PrintSupportedTypes.txt");
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

  return 0;
}
