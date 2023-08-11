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

std::string read_type_name(const ImageHdu& hdu) {
  const auto& id = hdu.read_typeid();
  ELEFITS_FOREACH_RASTER_TYPE(RETURN_TYPENAME_IF_MATCH)
  return "UNKNOWN TYPE";
}

std::string read_compression_name(const ImageHdu& hdu) {

  if (not hdu.is_compressed()) {
    return "None";
  }

  const auto algo = hdu.read_compression();
  const std::string losslessness = algo->is_lossless() ? "Lossless " : "Lossy ";

  if (dynamic_cast<Gzip*>(algo.get())) {
    return losslessness + "GZIP";
  }

  if (dynamic_cast<ShuffledGzip*>(algo.get())) {
    return losslessness + "Shuffled GZIP";
  }

  if (dynamic_cast<Rice*>(algo.get())) {
    return losslessness + "Rice";
  }

  if (dynamic_cast<HCompress*>(algo.get())) {
    return losslessness + "H-compress";
  }

  if (dynamic_cast<Plio*>(algo.get())) {
    return losslessness + "PLIO";
  }

  return "Unknown";
}

KeywordCategory parse_keyword_categories(const std::string& filter) {
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
    options.named("columns,C", value<long>()->default_value(0)->implicit_value(-1), "Maximum number of column names");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsReadStructure");

    /* Read options */
    const auto filename = args["input"].as<std::string>();
    const auto keyword_filter = args["keywords"].as<std::string>();
    const auto max_column_count = args["columns"].as<long>();
    KeywordCategory categories = parse_keyword_categories(keyword_filter);

    /* Read file */
    MefFile f(filename, FileMode::Read);
    const auto hdu_count = f.hdu_count();
    logger.info() << "HDU count: " << hdu_count;

    /* Loop over HDUs */
    for (long i = 0; i < hdu_count; ++i) {
      logger.info();

      /* Read name (if present) */
      const auto& hdu = f[i];
      logger.info() << "HDU #" << i << ": " << hdu.read_name();
      logger.info() << "  Size: " << hdu.size_in_file() << " bytes";

      /* Read type */
      const auto hduType = hdu.type(); // FIXME use category() to distinguish metadata from image HDUs
      if (hduType == HduCategory::Image) {
        const auto shape = hdu.as<ImageHdu>().read_shape<-1>();
        if (shape.size() > 0) {
          std::ostringstream oss;
          std::copy(shape.begin(), shape.end() - 1, std::ostream_iterator<int>(oss, " x "));
          oss << shape.container().back();
          logger.info() << "  Image HDU:";
          logger.info() << "    Type: " << read_type_name(hdu.as<ImageHdu>());
          logger.info() << "    Shape: " << oss.str() << " px";
          logger.info() << "    Compression: " << read_compression_name(hdu.as<ImageHdu>());
        } else {
          logger.info() << "  Metadata HDU";
        }
      } else {
        const auto column_count = hdu.as<BintableHdu>().read_column_count();
        const auto row_count = hdu.as<BintableHdu>().read_row_count();
        logger.info() << "  Binary table HDU:";
        logger.info() << "    Shape: " << column_count << " columns x " << row_count << " rows";
        if (max_column_count != 0) {
          auto column_names = hdu.as<BintableHdu>().columns().read_all_names();
          if (max_column_count > 0 && max_column_count < column_count) {
            column_names.resize(max_column_count);
            column_names.push_back("...");
          }
          logger.info() << "    Columns:";
          for (const auto& n : column_names) {
            logger.info() << "      " << n;
          }
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
