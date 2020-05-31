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

#include "EL_FitsFile/SifFile.h"
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
        ("catalog-in", value<std::string>()->default_value(""), "Input universe catalog")
        ("catalog-out", value<std::string>()->default_value(""), "Output universe catalog")
        ("sources", value<long>()->default_value(100), "Number of sources")
        ("observation", value<std::string>()->default_value("/tmp/obs.fits"), "Output observation")
        ("memory", value<std::string>()->default_value("/tmp/mmap.fits"), "Output memory map");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {
    const auto input_cat = args["catalog-in"].as<std::string>();
    const auto output_cat = args["catalog-out"].as<std::string>();
    const auto n = args["sources"].as<long>();
    Example::DataOriented::Universe universe;
    if(input_cat.empty()) {
      logger.info() << "Generating random universe...";
      universe.random(n);
    } else {
      logger.info() << "Loading universe...";
      universe.load(input_cat);
    }
    Example::Observation obs;
    logger.info() << "Rendering sources...";
    for(const auto& s : universe.sources())
      obs.draw(s.thumbnail, s.ra_dec);
    logger.info() << "Saving observation...";
    obs.save(args["observation"].as<std::string>());
    logger.info() << "Done!";
    if(not output_cat.empty()) {
      logger.info() << "Saving catalog...";
      universe.save(output_cat);
    }
    logger.info() << "Saving memory map...";
    SifFile file(args["memory"].as<std::string>(), SifFile::Permission::CREATE);
    file.write_raster(universe.memory_map());
    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_DataOrientedDesign)
