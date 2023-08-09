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

int getBitpix(const Fits::ImageHdu& hdu) {
  ELEFITS_FOREACH_RASTER_TYPE(IF_TYPEID_MATCHES_RETURN_BITPIX)
  return 0;
}

std::string readAlgoName(const Fits::ImageHdu& hdu) {

  if (not hdu.isCompressed()) {
    return "NONE";
  }

  const auto algo = hdu.readCompression();
  const std::string losslessness = algo->is_lossless() ? "Lossless " : "Lossy ";

  if (dynamic_cast<Fits::Gzip*>(algo.get())) {
    return losslessness + "GZIP";
  }

  if (dynamic_cast<Fits::ShuffledGzip*>(algo.get())) {
    return losslessness + "SHUFFLEDGZIP";
  }

  if (dynamic_cast<Fits::Rice*>(algo.get())) {
    return losslessness + "RICE";
  }

  if (dynamic_cast<Fits::HCompress*>(algo.get())) {
    return losslessness + "HCOMPRESS";
  }

  if (dynamic_cast<Fits::Plio*>(algo.get())) {
    return losslessness + "PLIO";
  }

  return "Unknown";
}

void setStrategy(Fits::MefFile& g, const std::string& testCase, bool lossy) {

  Fits::Quantization q(lossy ? Fits::Tile::rms / 16 : 0);
  Fits::Scaling s(lossy ? Fits::Tile::rms * 2.5 : 0);
  Fits::Plio plio(Fits::Tile::rowwise(), q);
  Fits::HCompress hc(Fits::Tile::rowwise(16), q, s);
  Fits::Rice rice(Fits::Tile::rowwise(), q);
  Fits::ShuffledGzip sgzip(Fits::Tile::rowwise(), q);
  Fits::Gzip gzip(Fits::Tile::rowwise(), q);

  if (testCase == "APTLY") {
    g.strategy(lossy ? Fits::CompressAptly(Fits::CompressionType::LosslessInts) : Fits::CompressAptly());
  } else if (testCase == "FULL") {
    g.strategy(std::move(plio), std::move(hc), std::move(rice), std::move(sgzip));
  } else if (testCase == "GZIP") {
    g.strategy(std::move(gzip));
  } else if (testCase == "SHUFFLEDGZIP") {
    g.strategy(std::move(sgzip));
  } else if (testCase == "RICE") {
    g.strategy(std::move(rice), std::move(sgzip));
  } else if (testCase == "HCOMPRESS") {
    g.strategy(std::move(hc), std::move(sgzip));
  } else if (testCase == "PLIO") {
    g.strategy(std::move(plio), std::move(sgzip));
  } else if (testCase != "NONE") {
    throw Fits::FitsError(std::string("Unknown test case: ") + testCase);
  }
}

/*
 * The program.
 */
class EleFitsCompressionExample : public Elements::Program {

public:
  // program options:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Fits::ProgramOptions options("Compress a FITS file using given strategy.");
    options.positional("input", value<std::string>(), "Input file");
    options.positional("output", value<std::string>()->default_value("/tmp/compressionBenchmark.fits"), "Output file");
    options.named(
        "case",
        value<std::string>()->default_value("GZIP"),
        "Compression strategy (NONE/FULL/APTLY/GZIP/SHUFFLEDGZIP/RICE/HCOMPRESS/PLIO)");
    options.flag("lossy", "Allow lossy compression");
    options.flag("extGZIP", "Apply external gzip to output file");
    options.named(
        "res",
        value<std::string>()->default_value("/tmp/compressionBenchmark.csv"),
        "File-level metrics output file");
    options.named(
        "resHdu",
        value<std::string>()->default_value("/tmp/compressionBenchmarkHdu.csv"),
        "HDU-level metrics output file");
    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const bool extGzip = args["extGZIP"].as<bool>();
    const auto filenameSrc = args["input"].as<std::string>();
    auto filenameDst = args["output"].as<std::string>();
    if (extGzip)
      filenameDst += ".gz";
    const auto testCase = args["case"].as<std::string>();
    const bool lossy = args["lossy"].as<bool>();
    const auto results = args["res"].as<std::string>();
    const auto resultsHdu = args["resHdu"].as<std::string>();

    Fits::Validation::CsvAppender writer(
        results,
        {"Filename",
         "Case",
         "Lossy",
         "ExtGZIP",
         "File size (bytes)",
         "Compressed size (bytes)",
         "Compression ratio",
         "Walltime (ms)",
         "HDU count",
         "HDU bitpixs",
         "Comptypes",
         "HDU sizes (bytes)",
         "HDU compressed sizes (bytes)",
         "HDU ratios",
         "Elapsed (ms)"});

    Fits::Validation::CsvAppender writerHdu(
        resultsHdu,
        {"Filename",
         "Case",
         "Lossy",
         "Bitpix",
         "Comptype",
         "HDU size (bytes)",
         "HDU compressed size (bytes)",
         "Compression ratio",
         "Elapsed (ms)",
         "Throughput (MB/s)"});

    Fits::Validation::Chronometer<std::chrono::milliseconds> chrono;
    Fits::Validation::Chronometer<std::chrono::milliseconds> chronoAll;
    int hduCounter = 0;
    std::vector<std::string> algos;
    std::vector<long> bitpixs;
    std::vector<std::size_t> hduSizes;
    std::vector<std::size_t> zHduSizes;
    std::vector<double> hduRatios;

    logger.info("Creating FITS file...");

    // Create mef file to write the extensions in
    chronoAll.start();
    Fits::MefFile f(filenameSrc, Fits::FileMode::Read);
    Fits::MefFile g(filenameDst, Fits::FileMode::Overwrite);
    setStrategy(g, testCase, lossy);

    // Copy without primary:
    // chrono.start();
    // for (const auto& hdu : f.filter<Fits::Hdu>(Fits::HduCategory::Ext)) {
    //   g.append(hdu);
    // }
    // chrono.stop();

    // Copy with primary (allows the primary to be compressed as well):
    logger.info("Compressing file...");
    const auto hduCount = f.hduCount();
    for (const auto& hdu : f) {

      long bitpix;
      std::string algo;
      long hduSize;
      long zHduSize;
      double ratio;

      if (hdu.type() == Fits::HduCategory::Bintable) {
        chrono.start();
        const auto& zHdu = g.append(hdu);
        chrono.stop();
        bitpix = 0;
        hduSize = hdu.size_in_file();
        zHduSize = zHdu.size_in_file();
        ratio = static_cast<double>(hduSize) / zHduSize;
        algo = "NONE";
        logger.info() << "HDU " << hdu.index() + 1 << "/" << hduCount << ": Uncompressed binary table";
      } else { // the hdu is an image
        chrono.start();
        const auto& zHdu = g.append(hdu);
        chrono.stop();
        bitpix = getBitpix(hdu.as<Fits::ImageHdu>());
        hduSize = hdu.size_in_file();
        zHduSize = zHdu.size_in_file();
        ratio = static_cast<double>(hduSize) / zHduSize;
        double throughput = static_cast<double>(hduSize) / chrono.last().count() / 1000; // converted from B/ms to MB/s
        // warning: if elapsed less than 1ms, divides by zero -> throughput infinite
        algo = readAlgoName(zHdu.as<Fits::ImageHdu>());
        writerHdu.writeRow(
            filenameSrc,
            testCase,
            lossy,
            bitpix,
            algo,
            hduSize,
            zHduSize,
            ratio,
            chrono.last().count(),
            throughput);
        logger.info() << "HDU " << hdu.index() + 1 << "/" << hduCount << ": " << algo;
      }

      bitpixs.push_back(bitpix);
      hduSizes.push_back(hduSize);
      zHduSizes.push_back(zHduSize);
      hduRatios.push_back(ratio);
      algos.push_back(algo);

      hduCounter++;
    }

    f.close();
    g.close();
    chronoAll.stop();

    long srcSize = boost::filesystem::file_size(filenameSrc);
    long dstSize = boost::filesystem::file_size(filenameDst);
    double compRatio = static_cast<double>(srcSize) / dstSize;

    writer.writeRow(
        filenameSrc,
        testCase,
        lossy,
        extGzip,
        srcSize,
        dstSize,
        compRatio,
        chronoAll.last().count(),
        hduCounter,
        join(bitpixs),
        joinString(algos),
        join(hduSizes),
        join(zHduSizes),
        join(hduRatios),
        join(chrono.increments()));

    logger.info("Done.");

    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompressionExample)