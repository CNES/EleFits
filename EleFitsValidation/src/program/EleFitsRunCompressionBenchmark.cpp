// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFits/MefFile.h"
#include "EleFitsData/HduCategory.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "EleFitsValidation/Chronometer.h"
#include "EleFitsValidation/CsvAppender.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <map>
#include <string>

using boost::program_options::value;
#include <boost/lexical_cast.hpp>

using namespace Euclid;

#define IF_TYPEID_MATCHES_RETURN_BITPIX(type, name) \
  if (typeid(type) == hdu.readTypeid()) \
    return Cfitsio::TypeCode<type>::bitpix();
// FIXME: return #name; as string instead ?

static Elements::Logging logger = Elements::Logging::getLogger("RunCompressionBenchmark");

template <typename T>
std::string join(const std::vector<T>& values, const std::string& sep = ",") {
  const auto begin = values.begin() + 1; // values[0] is used as initial value of accumulator
  const auto end = values.end();
  const auto init = std::to_string(values[0]);
  return std::accumulate(begin, end, init, [&](const std::string& a, T b) {
    return a + sep + std::to_string(b);
  });
}

template <typename T>
std::string joinString(const std::vector<T>& values, const std::string& sep = ",") {
  const auto begin = values.begin() + 1; // values[0] is used as initial value of accumulator
  const auto end = values.end();
  const auto init = values[0];
  return std::accumulate(begin, end, init, [&](const std::string& a, T b) {
    return a + sep + b;
  });
}

int getBitpix(Fits::ImageHdu hdu) {
  ELEFITS_FOREACH_RASTER_TYPE(IF_TYPEID_MATCHES_RETURN_BITPIX)
  return 0;
}

std::string readAlgoName(const Fits::ImageHdu& hdu) {

  if (not hdu.isCompressed()) {
    return "NONE";
  }

  const auto algo = hdu.readCompression();
  //const std::string losslessness = algo->isLossless() ? "Lossless " : "Lossy ";

  // if (dynamic_cast<Fits::Gzip*>(algo.get())) {
  //   return "GZIP";
  // }

  // if (dynamic_cast<Fits::ShuffledGzip*>(algo.get())) {
  //   return "SHUFFLEDGZIP";
  // }

  // if (dynamic_cast<Fits::Rice*>(algo.get())) {
  //   return "RICE";
  // }

  // if (dynamic_cast<Fits::HCompress*>(algo.get())) {
  //   return "HCOMPRESS";
  // }

  // if (dynamic_cast<Fits::Plio*>(algo.get())) {
  //   return "PLIO";
  // }

  return "Unknown";
}

// FIXME: get isLossless elsewhere
void setCompressionFromName(Fits::MefFile& g, std::string algoName) {

  if (algoName == "NONE") {
    g.stopCompressing();

  } else if (algoName == "RICE") {
    Fits::Rice algo;
    g.startCompressing(std::make_unique<Fits::FallbackCompressionStrategy<Fits::Rice>>(algo));

  } else if (algoName == "HCOMPRESS") {
    Fits::HCompress algo;
    g.startCompressing(std::make_unique<Fits::FallbackCompressionStrategy<Fits::HCompress>>(algo));

  } else if (algoName == "PLIO") {
    Fits::Plio algo;
    g.startCompressing(std::make_unique<Fits::FallbackCompressionStrategy<Fits::Plio>>(algo));

  } else if (algoName == "GZIP") {
    Fits::Gzip algo;
    g.startCompressing(algo);

  } else if (algoName == "SHUFFLEDGZIP") {
    Fits::ShuffledGzip algo;
    g.startCompressing(algo);

  } else {
    logger.info("# UNKNOWN COMPRESSION TYPE");
    logger.info("(disabling compression)");
    g.stopCompressing();
  }
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
        "case",
        value<std::string>()->default_value("GZIP"),
        "Compression algorithm name (RICE/HCOMPRESS/PLIO/GZIP/SHUFFLEDGZIP)");
    options.named("res", value<std::string>()->default_value("/tmp/compressionBenchmark.csv"), "Output result file");
    options.named(
        "resHdu",
        value<std::string>()->default_value("/tmp/compressionBenchmarkHdu.csv"),
        "Output result file per Hdu");
    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const auto filenameSrc = args["input"].as<std::string>();
    const auto filenameDst = args["output"].as<std::string>();
    const auto algoName = args["case"].as<std::string>();
    const auto results = args["res"].as<std::string>();
    const auto resultsHdu = args["resHdu"].as<std::string>();

    Fits::Validation::CsvAppender writer(
        results,
        {"Filename",
         "Case",
         "File size (bytes)",
         "Compressed size (bytes)",
         "HDU count",
         "HDU bitpixs",
         "Comptypes",
         "HDU sizes (bytes)",
         "HDU compressed sizes (bytes)",
         "Elapsed (ms)"});

    Fits::Validation::CsvAppender writerHdu(
        resultsHdu,
        {"Filename", "Case", "Bitpix", "Comptype", "HDU size (bytes)", "HDU compressed size (bytes)", "Elapsed (ms)"});

    logger.info("# Creating FITS file");

    // Create mef file to write the extensions in
    Fits::MefFile f(filenameSrc, Fits::FileMode::Read);
    Fits::MefFile g(filenameDst, Fits::FileMode::Overwrite);

    logger.info("# setting compression to " + algoName);
    setCompressionFromName(g, algoName);

    Fits::Validation::Chronometer<std::chrono::milliseconds> chrono;
    int hduCounter = 0;
    std::vector<std::string> actualAlgos;
    std::vector<long> bitpixs;
    std::vector<std::size_t> hduSizes;
    std::vector<std::size_t> zHduSizes;

    // Copy without primary:
    // chrono.start();
    // for (const auto& hdu : f.filter<Fits::Hdu>(Fits::HduCategory::Ext)) {
    //   g.appendCopy(hdu);
    // }
    // chrono.stop();

    // Copy with primary (allows the primary to be compressed as well):
    logger.info("# Compressing file..");
    for (const auto& hdu : f) {

      long bitpix;
      std::string actualAlgo;
      long hduSize;
      long zHduSize;

      if (hdu.matches(Fits::HduCategory::Bintable)) {
        chrono.start();
        auto zHdu = g.appendCopy(hdu);
        chrono.stop();
        bitpix = 0; // FIXME: what is the bitpix of bintable ?
        hduSize = hdu.readSizeInFile();
        zHduSize = zHdu.readSizeInFile();
        actualAlgo = "NONE";

      } else { // the hdu is an image

        chrono.start();
        auto zHdu = g.appendCopy(hdu);
        chrono.stop();
        bitpix = getBitpix(hdu.as<Fits::ImageHdu>());
        hduSize = hdu.readSizeInFile();
        zHduSize = zHdu.readSizeInFile();
        actualAlgo = readAlgoName(zHdu.as<Fits::ImageHdu>()); // zHdu.as() throws std::bad_cast
        logger.info("# HEY6");

        // {"Filename", "Case", "Bitpix", "Comptype", "HDU size (bytes)", "HDU compressed size (bytes)", "Elapsed (ms)"}
        writerHdu.writeRow(filenameSrc, algoName, bitpix, actualAlgo, hduSize, zHduSize, chrono.last().count());
      }

      bitpixs.push_back(bitpix);
      hduSizes.push_back(hduSize);
      zHduSizes.push_back(zHduSize);
      actualAlgos.push_back(actualAlgo);

      hduCounter++;
    }

    f.close();
    g.close();

    // {"Filename",
    //  "Case",
    //  "File size (bytes)",
    //  "Compressed size (bytes)",
    //  "HDU count",
    //  "HDU bitpixs",
    //  "Comptypes",
    //  "HDU sizes (bytes)",
    //  "HDU compressed sizes (bytes)",
    //  "Elapsed (ms)"});
    writer.writeRow(
        filenameSrc,
        algoName,
        boost::filesystem::file_size(filenameSrc),
        boost::filesystem::file_size(filenameDst),
        hduCounter,
        join(bitpixs),
        joinString(actualAlgos),
        join(hduSizes),
        join(zHduSizes),
        join(chrono.increments()));

    logger.info("# Compressed file created");

    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompressionExample)