// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestColumn.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "EleFitsValidation/Benchmark.h"
#include "EleFitsValidation/CfitsioBenchmark.h"
#include "EleFitsValidation/CsvAppender.h"
#include "EleFitsValidation/EleFitsBenchmark.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid::Fits;

Validation::BenchmarkFactory init_factory() {
  Validation::BenchmarkFactory factory;
  factory.register_benchmark<Validation::CfitsioBenchmark>("CFITSIO row-wise", 1);
  factory.register_benchmark<Validation::CfitsioBenchmark>("CFITSIO column-wise", -1);
  factory.register_benchmark<Validation::CfitsioBenchmark>("CFITSIO optimal", 0);
  factory.register_benchmark<Validation::EleFitsColwiseBenchmark>("EleFits column-wise");
  factory.register_benchmark<Validation::EleFitsBenchmark>("EleFits optimal");
  return factory;
}

template <typename T>
std::string join(const std::vector<T>& values, const std::string& sep = ",") {
  const auto begin = values.begin() + 1; // values[0] is used as initial value of accumulator
  const auto end = values.end();
  const auto init = std::to_string(values[0]);
  return std::accumulate(begin, end, init, [&](const std::string& a, T b) {
    return a + sep + std::to_string(b);
  });
}

class EleFitsRunBenchmark : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    ProgramOptions options;
    options.named("setup", value<std::string>()->default_value("EleFits optimal"), "Test setup to be benchmarked");
    options.named("images", value<int>()->default_value(0), "Number of image extensions");
    options.named("pixels", value<int>()->default_value(1), "Number of pixels");
    options.named("tables", value<int>()->default_value(0), "Number of binary table extensions");
    options.named("rows", value<int>()->default_value(1), "Number of rows");
    options.named("output", value<std::string>()->default_value("/tmp/test.fits"), "Output FITS file");
    options.named("res", value<std::string>()->default_value("/tmp/benchmark.csv"), "Output result file");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsRunBenchmark");

    const auto test_setup = args["setup"].as<std::string>();
    const auto image_count = args["images"].as<int>();
    const auto pixelCount = args["pixels"].as<int>();
    const auto table_count = args["tables"].as<int>();
    const auto row_count = args["rows"].as<int>();
    const auto filename = args["output"].as<std::string>();
    const auto results = args["res"].as<std::string>();

    logger.info("Setting up the benchmark...");

    const auto factory = init_factory(); // TODO pass factory to CTor of the program, with initFactory() as default
    for (const auto& k : factory.keys()) {
      logger.info(k);
    }
    auto benchmark = factory.create_benchmark(test_setup, filename);
    if (not benchmark) {
      throw Validation::TestCaseNotImplemented(std::string("No setup named: ") + test_setup);
    }
    Validation::CsvAppender writer(
        results,
        {"Date",
         "Test setup",
         "Mode",
         "HDU type",
         "HDU count",
         "Value count / HDU",
         "Total value count",
         "File size (bytes)",
         "Elapsed (ms)",
         "Min (ms)",
         "Max (ms)",
         "Mean (ms)",
         "Standard deviation (ms)",
         "Samples (ms)"});

    if (image_count) {

      logger.info("Generating raster...");

      const Validation::BRaster raster = Test::RandomRaster<std::int64_t, 1>({pixelCount});

      logger.info("Writing image HDUs...");

      try {
        const auto chrono = benchmark->write_images(image_count, raster);
        writer.writeRow(
            "TODO",
            test_setup,
            "Write",
            "Image",
            image_count,
            pixelCount,
            image_count * pixelCount,
            boost::filesystem::file_size(filename),
            chrono.elapsed().count(),
            chrono.min(),
            chrono.max(),
            chrono.mean(),
            chrono.stdev(),
            join(chrono.increments()));
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

      logger.info("Reading image HDUs...");

      try {
        const auto chrono = benchmark->read_images(1, image_count);
        writer.writeRow(
            "TODO",
            test_setup,
            "Read",
            "Image",
            image_count,
            pixelCount,
            image_count * pixelCount,
            boost::filesystem::file_size(filename),
            chrono.elapsed().count(),
            chrono.min(),
            chrono.max(),
            chrono.mean(),
            chrono.stdev(),
            join(chrono.increments()));
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

    } else if (table_count) {

      logger.info("Generating columns...");

      const auto table = Test::RandomTable(1, row_count);
      const Validation::BColumns columns = std::make_tuple(
          std::move(table.get_column<unsigned char>()),
          std::move(table.get_column<std::int32_t>()),
          std::move(table.get_column<std::int64_t>()),
          std::move(table.get_column<float>()),
          std::move(table.get_column<double>()),
          std::move(table.get_column<std::complex<float>>()),
          std::move(table.get_column<std::complex<double>>()),
          std::move(table.get_column<char>()),
          std::move(table.get_column<std::uint32_t>()),
          std::move(table.get_column<std::uint64_t>()));

      logger.info("Writing binary table HDUs...");

      try {
        const auto chrono = benchmark->write_bintables(table_count, columns);
        writer.writeRow(
            "TODO",
            test_setup,
            "Write",
            "Binary table",
            table_count,
            row_count * Validation::ColumnCount,
            table_count * row_count * Validation::ColumnCount,
            boost::filesystem::file_size(filename),
            chrono.elapsed().count(),
            chrono.min(),
            chrono.max(),
            chrono.mean(),
            chrono.stdev(),
            join(chrono.increments()));
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

      logger.info("Reading binary table HDUs...");

      try {
        const auto chrono = benchmark->read_bintables(1 + image_count, table_count);
        writer.writeRow(
            "TODO",
            test_setup,
            "Read",
            "Binary table",
            table_count,
            row_count * Validation::ColumnCount,
            table_count * row_count * Validation::ColumnCount,
            boost::filesystem::file_size(filename),
            chrono.elapsed().count(),
            chrono.min(),
            chrono.max(),
            chrono.mean(),
            chrono.stdev(),
            join(chrono.increments()));
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

    } else {
      throw Validation::TestCaseNotImplemented(
          "There should be either a positive number of image HDUs or a positive number of binary table HDUs");
    }

    logger.info("Done.");

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsRunBenchmark)
