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

class EleFitsCompress : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    auto options = ProgramOptions::fromAuxFile("Compress.txt");
    options.positional("input", value<std::string>(), "Input file");
    options.positional("output", value<std::string>(), "Output file (if ends with .gz, compress externally)");
    options.named("lossless", value<char>()->default_value('y'), "Losslessness: yes (y), no (n), integers only (i)");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsCompress");

    /* Read options */
    const auto input = args["input"].as<std::string>();
    const auto output = args["output"].as<std::string>();
    const auto lossless = args["lossless"].as<char>();

    /* Open files */
    MefFile raw(input, FileMode::Read);
    const auto hdu_count = raw.hduCount();
    logger.info() << "HDU count: " << hdu_count;

    MefFile compressed(output, FileMode::Create);
    if (lossless == 'y') {
      compressed.strategy(CompressAptly());
    } else if (lossless == 'i') {
      compressed.strategy(CompressAptly(CompressionType::LosslessInt));
    } else if (lossless == 'n') {
      compressed.strategy(CompressAptly(CompressionType::Lossy));
    } else {
      throw FitsError("Unknown compression type");
    }

    // FIXME handle Primary

    /* Loop over HDUs */
    for (long i = 1; i < hdu_count; ++i) {

      /* Read name (if present) */
      const auto& hdu = raw.access<>(i);
      logger.info() << "  HDU #" << i << ": " << hdu.readName();

      /* Copy-compress */
      // compressed.appendCopy(hdu);

// FIXME rm when appendCopy() supports compression
// FIXME copy records
#define ELEFITS_COPY_IMAGE_HDU(T, name) \
  if (image.readTypeid() == typeid(T)) { \
    const auto raster = image.raster().read<T, -1>(); \
    if (raster.size()) { \
      compressed.appendImage(hdu.readName(), {}, raster); \
    } else { \
      compressed.appendImageHeader<T>(hdu.readName(), {}); \
    } \
  }

      if (hdu.type() == HduCategory::Image) {
        const auto& image = hdu.as<ImageHdu>();
        ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_COPY_IMAGE_HDU)
      } else {
        compressed.appendCopy(hdu);
      }
      const auto& h = compressed.access<Header>(-1);
      h.writeSeq(hdu.header().parseAll(~KeywordCategory::Mandatory & ~KeywordCategory::Reserved));
    }

    raw.close();
    compressed.close();
    logger.info("Done.");

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompress)
