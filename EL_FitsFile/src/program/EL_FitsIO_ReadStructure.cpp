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

#include <stdexcept>
#include <map>
#include <ostream>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>

#include "ElementsKernel/ProgramHeaders.h"

#include "EL_FitsFile/MefFile.h"

using boost::program_options::bool_switch;
using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid::FitsIO;

#define RETURN_TYPENAME_IF_MATCH(type, name) \
  if (Euclid::Cfitsio::TypeCode<type>::bitpix() == bitpix) { \
    return #name; \
  }

std::string readBitpixName(const RecordHdu &hdu) {
  const int bitpix = hdu.parseRecord<int>("BITPIX");
  EL_FITSIO_FOREACH_RASTER_TYPE(RETURN_TYPENAME_IF_MATCH)
  return "UNKNOWN TYPE";
}

class EL_FitsIO_ReadStructure : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    add("input", value<std::string>(), "Input file");
    add("keywords",
        value<std::vector<bool>>()
            ->default_value({ false, false, false, false }, "0 0 0 0")
            ->multitoken()
            ->zero_tokens(),
        "Keyword mask: mandatory, reserved, comment, user (e.g. --keywords 0 0 0 1 displays only user keywords)");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_ReadStructure");

    /* Read options */
    const auto filename = args["input"].as<std::string>();
    const auto keywordBits = args["keywords"].as<std::vector<bool>>();
    const auto size = keywordBits.size();
    if (size != 0 && size != 4) {
      throw std::invalid_argument("keywords option takes 0 or 4 parameters");
    }
    RecordHdu::KeywordCategory keywordMask = RecordHdu::KeywordCategory::None;
    if (size == 0) {
      keywordMask = RecordHdu::KeywordCategory::All;
    } else {
      if (keywordBits[0]) {
        keywordMask |= RecordHdu::KeywordCategory::Mandatory;
      }
      if (keywordBits[1]) {
        keywordMask |= RecordHdu::KeywordCategory::Reserved;
      }
      if (keywordBits[2]) {
        keywordMask |= RecordHdu::KeywordCategory::Comment;
      }
      if (keywordBits[3]) {
        keywordMask |= RecordHdu::KeywordCategory::User;
      }
    }

    /* Read file */
    MefFile f(filename, FitsFile::Permission::Read);
    const auto hduCount = f.hduCount();
    logger.info() << "HDU count: " << hduCount;

    /* Loop over HDUs */
    for (long i = 0; i < hduCount; ++i) {
      logger.info();

      /* Read name (if present) */
      const auto &hdu = f.access<>(i);
      logger.info() << "HDU #" << i << ": " << hdu.readName();

      /* Read type */
      const auto hduType = hdu.type();
      if (hduType == HduType::Image) {
        const auto shape = dynamic_cast<const ImageHdu *>(&hdu)->readShape<-1>();
        if (shape.size() > 0) {
          std::ostringstream oss;
          std::copy(shape.begin(), shape.end() - 1, std::ostream_iterator<int>(oss, " x "));
          oss << shape.back();
          logger.info() << "  Image HDU:";
          logger.info() << "    Type: " << readBitpixName(hdu);
          logger.info() << "    Shape: " << oss.str() << " px";
        } else {
          logger.info() << "  Metadata HDU";
        }
      } else {
        const auto columnCount = dynamic_cast<const BintableHdu *>(&hdu)->readColumnCount();
        const auto rowCount = dynamic_cast<const BintableHdu *>(&hdu)->readRowCount();
        logger.info() << "  Binary table HDU:";
        logger.info() << "    Shape: " << columnCount << " columns x " << rowCount << " rows";
      }

      /* Read keywords */
      if (keywordMask) {
        const auto keywords = hdu.readKeywords(keywordMask);
        if (keywords.size() == 0) {
          logger.info() << "  No keywords";
        } else {
          logger.info() << "  Keywords:";
          for (const auto &k : keywords) {
            logger.info() << "    " << k;
          }
        }
      }
    }

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_ReadStructure)
