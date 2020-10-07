/**
 * @file src/program/EL_FitsIO_PrintSupportedTypes.cpp
 * @date 10/07/20
 * @author user
 *
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

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Unused.h"

#include "EL_FitsData/TestColumn.h"
#include "EL_FitsData/TestRaster.h"
#include "EL_FitsData/TestRecord.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;

#define PRINT_SUPPORTED_TYPES(type, name) logger.info() << "  " << #type;

class EL_FitsIO_PrintSupportedTypes : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    return options;
  }

  Elements::ExitCode mainMethod(ELEMENTS_UNUSED std::map<std::string, variable_value> &args) override {
    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_PrintSupportedTypes");
    logger.info("----------------------------");
    logger.info("Supported Record value types");
    logger.info("----------------------------");
    EL_FITSIO_FOREACH_RECORD_TYPE(PRINT_SUPPORTED_TYPES)
    logger.info("----------------------------");
    logger.info("Supported Raster value types");
    logger.info("----------------------------");
    EL_FITSIO_FOREACH_RASTER_TYPE(PRINT_SUPPORTED_TYPES)
    logger.info("----------------------------");
    logger.info("Supported Column value types");
    logger.info("----------------------------");
    EL_FITSIO_FOREACH_COLUMN_TYPE(PRINT_SUPPORTED_TYPES)
    logger.info("----------------------------");
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_PrintSupportedTypes)
