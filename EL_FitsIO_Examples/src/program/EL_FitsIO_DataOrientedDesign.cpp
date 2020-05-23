/**
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

#include "EL_FitsIO_Examples/DodUniverse.h"
#include "EL_FitsIO_Examples/Observation.h"

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variable_value;

using namespace Euclid::FitsIO;

static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_DataOrientedDesign");

class EL_FitsIO_DataOrientedDesign : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
    options_description options {};
    options.add_options()
        ("catalog", value<std::string>()->default_value(""), "Input universe catalog")
        ("sources", value<long>()->default_value(100), "Number of sources")
        ("width", value<long>()->default_value(100), "Observation width")
        ("height", value<long>()->default_value(100), "Observation height")
        ("observation", value<std::string>()->default_value("/tmp/obs.fits"), "Output observation");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {
    const auto cat = args["catalog"].as<std::string>();
    const auto n = args["sources"].as<long>();
    const auto w = args["width"].as<long>();
    const auto h = args["height"].as<long>();
    DataOriented::Universe universe;
    if(cat.empty()) {
      logger.info() << "Generating random universe...";
      universe.random(n, w, h);
    } else {
      logger.info() << "Loading universe...";
      universe.load(cat);
    }
    Observation obs({w, h});
    logger.info() << "Rendering sources...";
    for(const auto& s : universe.sources())
      obs.draw(s.thumbnail, s.ra, s.dec);
    logger.info() << "Saving observation...";
    obs.save(args["observation"].as<std::string>());
    logger.info() << "Done!";
    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_DataOrientedDesign)
