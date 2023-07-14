// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <iomanip> // setw, setfill
#include <ostream>
#include <sstream>
#include <string>

using boost::program_options::value;
using namespace Euclid::Fits;

#define RETURN_TYPENAME_IF_MATCH(type, name) \
  if (typeid(type) == id) { \
    return #name; \
  }

std::string readBitpixName(const ImageHdu& hdu) {
  const auto& id = hdu.readTypeid();
  ELEFITS_FOREACH_RASTER_TYPE(RETURN_TYPENAME_IF_MATCH)
  return "UNKNOWN TYPE";
}

std::string readAlgoName(const ImageHdu& hdu) {

  if (not hdu.isCompressed()) {
    return "None";
  }

  auto algo = hdu.readCompression();

  if (dynamic_cast<Gzip*>(algo.get())) {
    return "GZIP";
  }

  if (dynamic_cast<ShuffledGzip*>(algo.get())) {
    return "Shuffled GZIP";
  }

  if (dynamic_cast<Rice*>(algo.get())) {
    return "Rice";
  }

  if (dynamic_cast<HCompress*>(algo.get())) {
    return "H-compress";
  }

  if (dynamic_cast<Plio*>(algo.get())) {
    return "PLIO";
  }

  return "Unknown";
}

KeywordCategory parseKeywordCategories(const std::string& filter) {
  auto categories = KeywordCategory::None;
  static const std::map<char, KeywordCategory> mapping {
      {'m', KeywordCategory::Mandatory},
      {'r', KeywordCategory::Reserved},
      {'c', KeywordCategory::Comment},
      {'u', KeywordCategory::User}};
  for (const auto& f : filter) {
    categories |= mapping.find(f)->second;
  }
  return categories;
}

class EleFitsReadStructure : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    auto options = ProgramOptions::fromAuxFile("ReadStructure.txt");
    options.positional("input", value<std::string>(), "Input file");
    options.named("keywords,K", value<std::string>()->default_value("")->implicit_value("mrcu"), "Record filter");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsReadStructure");

    /* Read options */
    const auto filename = args["input"].as<std::string>();
    const auto keywordFilter = args["keywords"].as<std::string>();
    KeywordCategory categories = parseKeywordCategories(keywordFilter);

    /* Read file */
    MefFile f(filename, FileMode::Read);
    const auto hduCount = f.hduCount();
    logger.info() << "HDU count: " << hduCount;

    /* Loop over HDUs */
    for (long i = 0; i < hduCount; ++i) {
      logger.info();

      /* Read name (if present) */
      const auto& hdu = f.access<>(i);
      logger.info() << "HDU #" << i << ": " << hdu.readName();

      /* Read type */
      const auto hduType = hdu.type(); // FIXME use category() to distinguish metadata from image HDUs
      if (hduType == HduCategory::Image) {
        const auto shape = hdu.as<ImageHdu>().readShape<-1>();
        if (shape.size() > 0) {
          std::ostringstream oss;
          std::copy(shape.begin(), shape.end() - 1, std::ostream_iterator<int>(oss, " x "));
          oss << shape.container().back();
          logger.info() << "  Image HDU:";
          logger.info() << "    Type: " << readBitpixName(hdu.as<ImageHdu>());
          logger.info() << "    Shape: " << oss.str() << " px";
          logger.info() << "    Compression: " << readAlgoName(hdu.as<ImageHdu>());
        } else {
          logger.info() << "  Metadata HDU";
        }
      } else {
        const auto columnCount = hdu.as<BintableHdu>().readColumnCount();
        const auto rowCount = hdu.as<BintableHdu>().readRowCount();
        logger.info() << "  Binary table HDU:";
        logger.info() << "    Shape: " << columnCount << " columns x " << rowCount << " rows";
        const auto columnNames = hdu.as<BintableHdu>().columns().readAllNames();
        logger.info() << "    Columns:";
        for (const auto& n : columnNames) {
          logger.info() << "      " << n;
        }
      }

      /* Read keywords */
      if (categories) {
        const auto records = hdu.header().readKeywordsValues(categories);
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

MAIN_FOR(EleFitsReadStructure)
