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
  if (typeid(type) == hdu.read_typeid()) \
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
std::string join_string(const std::vector<T>& values, const std::string& sep = ",") {
  const auto begin = values.begin() + 1; // values[0] is used as initial value of accumulator
  const auto end = values.end();
  const auto init = values[0];
  return std::accumulate(begin, end, init, [&](const std::string& a, T b) {
    return a + sep + b;
  });
}

int read_bitpix(const Fits::ImageHdu& hdu) {
  ELEFITS_FOREACH_RASTER_TYPE(IF_TYPEID_MATCHES_RETURN_BITPIX)
  return 0;
}

std::string read_algo_name(const Fits::ImageHdu& hdu) {

  if (not hdu.is_compressed()) {
    return "NONE";
  }

  const auto algo = hdu.read_compression();
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

void set_strategy(Fits::MefFile& g, const std::string& test_case, bool lossy) {

  Fits::Quantization q(lossy ? Fits::Tile::rms / 16 : 0);
  Fits::Scaling s(lossy ? Fits::Tile::rms * 2.5 : 0);
  Fits::Plio plio(Fits::Tile::rowwise(), q);
  Fits::HCompress hc(Fits::Tile::rowwise(16), q, s);
  Fits::Rice rice(Fits::Tile::rowwise(), q);
  Fits::ShuffledGzip sgzip(Fits::Tile::rowwise(), q);
  Fits::Gzip gzip(Fits::Tile::rowwise(), q);

  if (test_case == "AUTO") {
    g.strategy(lossy ? Fits::CompressAuto(Fits::CompressionType::Lossy) : Fits::CompressAuto());
  } else if (test_case == "FULL") {
    g.strategy(std::move(plio), std::move(hc), std::move(rice), std::move(sgzip));
  } else if (test_case == "GZIP") {
    g.strategy(std::move(gzip));
  } else if (test_case == "SHUFFLEDGZIP") {
    g.strategy(std::move(sgzip));
  } else if (test_case == "RICE") {
    g.strategy(std::move(rice), std::move(sgzip));
  } else if (test_case == "HCOMPRESS") {
    g.strategy(std::move(hc), std::move(sgzip));
  } else if (test_case == "PLIO") {
    g.strategy(std::move(plio), std::move(sgzip));
  } else if (test_case != "NONE") {
    throw Fits::FitsError(std::string("Unknown test case: ") + test_case);
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
        "Compression strategy (NONE/FULL/AUTO/GZIP/SHUFFLEDGZIP/RICE/HCOMPRESS/PLIO)");
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
    return options.as_pair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const bool ext_gzip = args["extGZIP"].as<bool>();
    const auto input = args["input"].as<std::string>();
    auto output = args["output"].as<std::string>();
    if (ext_gzip)
      output += ".gz";
    const auto test_case = args["case"].as<std::string>();
    const bool lossy = args["lossy"].as<bool>();
    const auto results = args["res"].as<std::string>();
    const auto results_hdu = args["resHdu"].as<std::string>();

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

    Fits::Validation::CsvAppender writer_hdu(
        results_hdu,
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
    Fits::Validation::Chronometer<std::chrono::milliseconds> walltime;
    long hdu_counter = 0;
    std::vector<std::string> algos;
    std::vector<long> bitpixs;
    std::vector<std::size_t> hdu_sizes;
    std::vector<std::size_t> z_hdu_sizes;
    std::vector<double> hdu_ratios;

    logger.info("Creating FITS file...");

    // Create mef file to write the extensions in
    walltime.start();
    Fits::MefFile f(input, Fits::FileMode::Read);
    Fits::MefFile g(output, Fits::FileMode::Overwrite);
    set_strategy(g, test_case, lossy);

    // Copy without primary:
    // chrono.start();
    // for (const auto& hdu : f.filter<Fits::Hdu>(Fits::HduCategory::Ext)) {
    //   g.append(hdu);
    // }
    // chrono.stop();

    // Copy with primary (allows the primary to be compressed as well):
    logger.info("Compressing file...");
    const auto hdu_count = f.hdu_count();
    for (const auto& hdu : f) {

      long bitpix;
      std::string algo;
      long hdu_size;
      long z_hdu_size;
      double ratio;

      if (hdu.type() == Fits::HduCategory::Bintable) {
        chrono.start();
        const auto& z_hdu = g.append(hdu);
        chrono.stop();
        bitpix = 0;
        hdu_size = hdu.size_in_file();
        z_hdu_size = z_hdu.size_in_file();
        ratio = static_cast<double>(hdu_size) / z_hdu_size;
        algo = "NONE";
        logger.info() << "HDU " << hdu.index() + 1 << "/" << hdu_count << ": Uncompressed binary table";
      } else { // the hdu is an image
        chrono.start();
        const auto& z_hdu = g.append(hdu);
        chrono.stop();
        bitpix = read_bitpix(hdu.as<Fits::ImageHdu>());
        hdu_size = hdu.size_in_file();
        z_hdu_size = z_hdu.size_in_file();
        ratio = static_cast<double>(hdu_size) / z_hdu_size;
        double throughput = static_cast<double>(hdu_size) / chrono.last().count() / 1000; // converted from B/ms to MB/s
        // warning: if elapsed less than 1ms, divides by zero -> throughput infinite
        algo = read_algo_name(z_hdu.as<Fits::ImageHdu>());
        writer_hdu.write_row(
            input,
            test_case,
            lossy,
            bitpix,
            algo,
            hdu_size,
            z_hdu_size,
            ratio,
            chrono.last().count(),
            throughput);
        logger.info() << "HDU " << hdu.index() + 1 << "/" << hdu_count << ": " << algo;
      }

      bitpixs.push_back(bitpix);
      hdu_sizes.push_back(hdu_size);
      z_hdu_sizes.push_back(z_hdu_size);
      hdu_ratios.push_back(ratio);
      algos.push_back(algo);

      hdu_counter++;
    }

    f.close();
    g.close();
    walltime.stop();

    long input_size = boost::filesystem::file_size(input);
    long output_size = boost::filesystem::file_size(output);
    double comp_ratio = static_cast<double>(input_size) / output_size;

    writer.write_row(
        input,
        test_case,
        lossy,
        ext_gzip,
        input_size,
        output_size,
        comp_ratio,
        walltime.last().count(),
        hdu_counter,
        join(bitpixs),
        join_string(algos),
        join(hdu_sizes),
        join(z_hdu_sizes),
        join(hdu_ratios),
        join(chrono.increments()));

    logger.info("Done.");

    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompressionExample)