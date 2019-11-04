/**
 * @file src/program/EL_FitsIO_WritePerf.cpp
 * @date 11/04/19
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

using boost::program_options::options_description;
using boost::program_options::variable_value;


static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_WritePerf");

class EL_FitsIO_WritePerf : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
  
    options_description options {};
    
    //
    // !!! Implement the program options here !!!
    //
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_WritePerf");

    logger.info("#");
    logger.info("# Entering mainMethod()");
    logger.info("#");

    //
    // !!! Implement you program here !!!
    //

    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_WritePerf)



