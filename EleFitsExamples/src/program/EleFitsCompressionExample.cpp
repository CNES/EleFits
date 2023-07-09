// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleFits/MefFile.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

//#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;

#define CREATE_IMAGE_FROM_TYPES(type, name) addGenericImageFromType<type>(f, #name, algoName, rows, cols);

static Elements::Logging logger = Elements::Logging::getLogger("EleFitsCompressionExample");

template <typename T>
bool areCompatible(std::string algoName) {

  if (algoName == "NONE")
    return true;

  // 64-bit integers are not supported by CFITSIO for compression
  if (typeid(T) == typeid(std::int64_t) || (typeid(T) == typeid(std::uint64_t)))
    return false;

  // PLIO_1 must be used for integer image types with values between 0 and 2^24.
  if (algoName == "PLIO") {

    // FIXME: this actually seems to depend on the size of images !!
    if (typeid(T) == typeid(std::uint32_t))
      return false;
  }

  // GZIP, SHUFFLEDGZIP and RICE are general purpose algorithms
  // TODO verify if HCOMPRESS (2D compression) works with any x-dimensional image or not
  return true;
}

template <typename T>
void addGenericImageFromType(Fits::MefFile& f, std::string typeName, std::string algoName, long rows, long cols) {

  logger.info("- Adding Image with type " + typeName);

  if (not areCompatible<T>(algoName))
    f.stopCompressing();

  // Create and add Raster with type T as extension:
  Fits::VecRaster<T, 2> raster2D({cols, rows});
  std::iota(raster2D.begin(), raster2D.end(), 0);
  f.assignImageExt(typeName + "_IMAGE", raster2D);
}

/*
 * The program.
 */
class EleFitsCompressionExample : public Elements::Program {

public:
  // program options:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Fits::ProgramOptions options("Write mef with all supported image extensions.");
    options.positional("output", value<std::string>()->default_value("/tmp/compression.fits"), "Output file");
    options.named("rows", value<long>()->default_value(20), "Number of rows");
    options.named("cols", value<long>()->default_value(30), "Number of columns");
    options.named(
        "comptype",
        value<std::string>()->default_value("NONE"),
        "Compression algorithm name (NONE/RICE/HCOMPRESS/PLIO/GZIP/SHUFFLEDGZIP)");

    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const auto filename = args["output"].as<std::string>();
    const auto rows = args["rows"].as<long>();
    const auto cols = args["cols"].as<long>();
    const auto algoName = args["comptype"].as<std::string>();

    logger.info("# Creating FITS file");

    // Create mef file to write the extensions in
    Fits::MefFile f(filename, Fits::FileMode::Overwrite);

    if (algoName == "NONE") {

      logger.info("# setting compression to None");
      Fits::None algo;
      f.startCompressing(algo);

    } else if (algoName == "RICE") {

      logger.info("# setting compression to Rice");
      Fits::Rice algo;
      f.startCompressing(algo);

    } else if (algoName == "HCOMPRESS") {

      logger.info("# setting compression to Hcompress");
      Fits::HCompress algo;
      f.startCompressing(algo);

    } else if (algoName == "PLIO") {

      logger.info("# setting compression to Plio");
      Fits::Plio algo;
      f.startCompressing(algo);

    } else if (algoName == "GZIP") {

      logger.info("# setting compression to Gzip");
      Fits::Gzip algo;
      f.startCompressing(algo);

    } else if (algoName == "SHUFFLEDGZIP") {

      logger.info("# setting compression to ShuffledGzip");
      Fits::ShuffledGzip algo;
      f.startCompressing(algo);

    } else {

      logger.info("# UNKNOWN COMPRESSION TYPE");
      logger.info("(disabling compression)");
      Fits::None algo;
      f.stopCompressing();
    }

    // Adds an image for each Raster type to the mef file
    ELEFITS_FOREACH_RASTER_TYPE(CREATE_IMAGE_FROM_TYPES)

    logger.info("# File created");

    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompressionExample)