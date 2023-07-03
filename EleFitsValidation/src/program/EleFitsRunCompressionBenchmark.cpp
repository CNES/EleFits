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

// template <typename T>
// bool areCompatible(std::string algoName) {

//   if (algoName == "NONE")
//     return true;

//   // 64-bit integers are not supported by CFITSIO for compression
//   if (typeid(T) == typeid(std::int64_t) || (typeid(T) == typeid(std::uint64_t)))
//     return false;

//   // PLIO_1 must be used for integer image types with values between 0 and 2^24.
//   if (algoName == "PLIO") {

//     // FIXME: this actually seems to depend on the size of images !!
//     if (typeid(T) == typeid(std::uint32_t))
//       return false;
//   }

//   // GZIP, SHUFFLEDGZIP and RICE are general purpose algorithms
//   // TODO verify if HCOMPRESS (2D compression) works with any x-dimensional image or not
//   return true;
// }

void setCompressionFromName(Fits::MefFile& g, std::string algoName) {
  if (algoName == "RICE") {
    Fits::Compression::Rice algo;
    g.startCompressing(algo);

  } else if (algoName == "HCOMPRESS") {
    Fits::Compression::HCompress algo;
    g.startCompressing(algo);

  } else if (algoName == "PLIO") {
    Fits::Compression::Plio algo;
    g.startCompressing(algo);

  } else if (algoName == "GZIP") {
    Fits::Compression::Gzip algo;
    g.startCompressing(algo);

  } else if (algoName == "SHUFFLEDGZIP") {
    Fits::Compression::ShuffledGzip algo;
    g.startCompressing(algo);

  } else {
    logger.info("# UNKNOWN COMPRESSION TYPE");
    logger.info("(disabling compression)");
    Fits::Compression::None algo;
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
        value<std::string>()->default_value("NONE"),
        "Compression algorithm name (RICE/HCOMPRESS/PLIO/GZIP/SHUFFLEDGZIP)");
    options.named("res", value<std::string>()->default_value("/tmp/compressionBenchmark.csv"), "Output result file");
    // options
    //     .named("resHdu", value<std::string>()->default_value("/tmp/compressionBenchmarkHdu.csv"), "Output result file");
    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const auto filenameSrc = args["input"].as<std::string>();
    const auto filenameDst = args["output"].as<std::string>();
    const auto algoName = args["case"].as<std::string>();
    const auto results = args["res"].as<std::string>();
    // const auto resultsHdu = args["resHdu"].as<std::string>();

    Fits::Validation::CsvAppender writer(
        results,
        {"Filename",
         "Case",
         "File size (bytes)",
         "Compressed size (bytes)",
         "HDU count",
         "HDUs bitpix",
         "Comptypes",
         "HDUs size (bytes)",
         "HDUs compressed size (bytes)",
         "Elapsed (ms)"});

    // Fits::Validation::CsvAppender writerHdu(
    //     resultsHdu,
    //     {"Filename",
    //      "Case",
    //      "Compressed size (bytes)",
    //      "HDU count",
    //      "Bitpixs",
    //      "Pixel counts",
    //      "Comptypes",
    //      "Elapsed (ms)"});

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
    std::vector<long> hduSizes;
    std::vector<long> zHduSizes;

    // Copy without primary:
    // chrono.start();
    // for (const auto& hdu : f.filter<Fits::Hdu>(Fits::HduCategory::Ext)) {
    //   g.appendCopy(hdu);
    // }
    // chrono.stop();

    // Copy with primary (allows the primary to be compressed as well):
    logger.info("# Compressing file..");
    for (const auto& hdu : f) {

      if (hdu.matches(Fits::HduCategory::Bintable)) {
        chrono.start();
        auto zHdu = g.appendCopy(hdu);
        chrono.stop();
        bitpixs.push_back(0); // FIXME what is the bitpix of bintable ?
        hduSizes.push_back(hdu.readDataUnitSize());
        zHduSizes.push_back(zHdu.readDataUnitSize());
        actualAlgos.push_back("NONE");

      } else { // the hdu is an image
        try {

          chrono.start();
          auto zHdu = g.appendCopy(hdu);
          chrono.stop();
          bitpixs.push_back(getBitpix(hdu.as<Fits::ImageHdu>()));
          // hduSizes.push_back(hdu.as<Fits::ImageHdu>().readSize());
          hduSizes.push_back(hdu.readDataUnitSize());
          zHduSizes.push_back(zHdu.readDataUnitSize());
          actualAlgos.push_back(algoName);

        } catch (const Cfitsio::CfitsioError&) {

          logger.info("# fallback to ShuffledGzip for current Hdu");
          Fits::Compression::ShuffledGzip defaultAlgo;
          g.startCompressing(defaultAlgo);

          chrono.start();
          auto zHdu = g.appendCopy(hdu);
          chrono.stop();
          bitpixs.push_back(getBitpix(hdu.as<Fits::ImageHdu>()));
          // hduSizes.push_back(hdu.as<Fits::ImageHdu>().readSize());
          hduSizes.push_back(hdu.readDataUnitSize());
          zHduSizes.push_back(zHdu.readDataUnitSize());
          actualAlgos.push_back("SHUFFLEDGZIP");

          setCompressionFromName(g, algoName);
        }
      }

      hduCounter++;
    }

    // {"Filename",
    //  "Case",
    //  "File size (bytes)",
    //  "Compressed size (bytes)",
    //  "HDU count",
    //  "HDUs bitpix",
    //  "Comptypes",
    //  "HDUs size (bytes)",
    //  "HDUs compressed size (bytes)",
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