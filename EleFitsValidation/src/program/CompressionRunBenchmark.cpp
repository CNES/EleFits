// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleFits/MefFile.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "EleFitsValidation/Chronometer.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;

static Elements::Logging logger = Elements::Logging::getLogger("CompressionRunBenchmark");

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

/*
 * The program.
 */
class EleFitsCompressionExample : public Elements::Program {

public:
  // program options:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Fits::ProgramOptions options("Write mef with all supported image extensions.");
    options.positional("input", value<std::string>(), "Input file");
    options.positional("output", value<std::string>()->default_value("/tmp/compressionBenchmark.fits"), "Output file");
    options.named(
        "comptype",
        value<std::string>()->default_value("NONE"),
        "Compression algorithm name (NONE/RICE/HCOMPRESS/PLIO/GZIP/SHUFFLEDGZIP)");

    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const auto filenameSrc = args["input"].as<std::string>();
    const auto filenameDst = args["output"].as<std::string>();
    const auto algoName = args["comptype"].as<std::string>();

    logger.info("# Creating FITS file");

    // Create mef file to write the extensions in
    Fits::MefFile f(filenameSrc, Fits::FileMode::Read);
    Fits::MefFile g(filenameDst, Fits::FileMode::Overwrite);

    if (algoName == "NONE") {
      logger.info("# setting compression to None");
      Fits::Compression::None algo;
      g.startCompressing(algo);

    } else if (algoName == "RICE") {
      logger.info("# setting compression to Rice");
      Fits::Compression::Rice algo(-Fits::Position<2>::one());
      g.startCompressing(algo);

    } else if (algoName == "HCOMPRESS") {
      logger.info("# setting compression to Hcompress");
      Fits::Compression::HCompress algo(-Fits::Position<2>::one());
      g.startCompressing(algo);

    } else if (algoName == "PLIO") {
      logger.info("# setting compression to Plio");
      Fits::Compression::Plio algo(-Fits::Position<2>::one());
      g.startCompressing(algo);

    } else if (algoName == "GZIP") {
      logger.info("# setting compression to Gzip");
      Fits::Compression::Gzip algo(-Fits::Position<2>::one());
      g.startCompressing(algo);

    } else if (algoName == "SHUFFLEDGZIP") {
      logger.info("# setting compression to ShuffledGzip");
      Fits::Compression::ShuffledGzip algo(-Fits::Position<2>::one());
      g.startCompressing(algo);

    } else {
      logger.info("# UNKNOWN COMPRESSION TYPE");
      logger.info("(disabling compression)");
      Fits::Compression::None algo;
      g.stopCompressing();
    }

    Fits::Validation::Chronometer<std::chrono::milliseconds> chrono;

    // Copy without primary:
    // chrono.start();
    // for (const auto& hdu : f.filter<Fits::Hdu>(Fits::HduCategory::Ext)) {
    //   g.appendCopy(hdu);
    // }
    // chrono.stop();

    // Copy with primary (allows the primary to be compressed as well):
    logger.info("# Compressing file..");
    chrono.start();
    for (const auto& hdu : f) {
      g.appendCopy(hdu);
    }
    chrono.stop();

    double timeSec = chrono.mean() / 1000.0;

    logger.info("# Compressed file created");
    logger.info() << "# Time (in sec): " << timeSec;

    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompressionExample)