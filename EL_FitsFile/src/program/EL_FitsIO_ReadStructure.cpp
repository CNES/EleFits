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

#include "EL_FitsFile/MefFile.h"
#include "EL_FitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <iomanip> // setw, setfill
#include <ostream>
#include <sstream>
#include <string>

using boost::program_options::value;
using namespace Euclid::FitsIO;

#define RETURN_TYPENAME_IF_MATCH(type, name) \
  if (typeid(type) == id) { \
    return #name; \
  }

std::string readBitpixName(const ImageHdu& hdu) {
  const auto& id = hdu.readTypeid();
  EL_FITSIO_FOREACH_RASTER_TYPE(RETURN_TYPENAME_IF_MATCH)
  return "UNKNOWN TYPE";
}

KeywordCategory parseKeywordCategories(const std::string& filter) {
  auto categories = KeywordCategory::None;
  static const std::map<char, KeywordCategory> mapping { { 'm', KeywordCategory::Mandatory },
                                                         { 'r', KeywordCategory::Reserved },
                                                         { 'c', KeywordCategory::Comment },
                                                         { 'u', KeywordCategory::User } };
  for (const auto& f : filter) {
    categories |= mapping.find(f)->second;
  }
  return categories;
}

class EL_FitsIO_ReadStructure : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    auto options = ProgramOptions::fromAuxFile("ReadStructure.txt");
    options.positional("input", value<std::string>(), "Input file");
    options.named("keywords,K", value<std::string>()->default_value("")->implicit_value("mrcu"), "Record filter");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_ReadStructure");

    /* Read options */
    const auto filename = args["input"].as<std::string>();
    const auto keywordFilter = args["keywords"].as<std::string>();
    KeywordCategory categories = parseKeywordCategories(keywordFilter);

    /* Read file */
    MefFile f(filename, FitsFile::Permission::Read);
    const auto hduCount = f.hduCount();
    logger.info() << "HDU count: " << hduCount;

    /* Loop over HDUs */
    for (long i = 0; i < hduCount; ++i) {
      logger.info();

      /* Read name (if present) */
      const auto& hdu = f.access<>(i);
      logger.info() << "HDU #" << i << ": " << hdu.readName();

      /* Read type */
      const auto hduType = hdu.type();
      if (hduType == HduType::Image) {
        const auto shape = hdu.as<ImageHdu>().readShape<-1>();
        if (shape.size() > 0) {
          std::ostringstream oss;
          std::copy(shape.begin(), shape.end() - 1, std::ostream_iterator<int>(oss, " x "));
          oss << shape.back();
          logger.info() << "  Image HDU:";
          logger.info() << "    Type: " << readBitpixName(hdu.as<ImageHdu>());
          logger.info() << "    Shape: " << oss.str() << " px";
        } else {
          logger.info() << "  Metadata HDU";
        }
      } else {
        const auto columnCount = hdu.as<BintableHdu>().readColumnCount();
        const auto rowCount = hdu.as<BintableHdu>().readRowCount();
        logger.info() << "  Binary table HDU:";
        logger.info() << "    Shape: " << columnCount << " columns x " << rowCount << " rows";
        const auto columnNames = hdu.as<BintableHdu>().readColumnNames();
        logger.info() << "    Columns:";
        for (const auto& n : columnNames) {
          logger.info() << "      " << n;
        }
      }

      /* Read keywords */
      if (categories) {
        const auto records = hdu.readKeywordsValues(categories);
        if (records.size() == 0) {
          logger.info() << "  No keywords";
        } else {
          logger.info() << "  Keywords:";
          for (const auto& r : records) {
            if (r.second.empty()) {
              logger.info() << "    " << r.first;
            } else {
              logger.info() << "    " << std::left << std::setw(8) << std::setfill(' ') << r.first << " = " << r.second;
            }
          }
        }
      }
    }

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_ReadStructure)
