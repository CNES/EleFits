/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <chrono>
#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "ElementsKernel/ProgramHeaders.h"

#include "EL_FitsData/TestColumn.h"
#include "EL_FitsData/TestRaster.h"

#include "EL_FitsIO_Validation/Benchmark.h"
#include "EL_FitsIO_Validation/CfitsioBenchmark.h"
#include "EL_FitsIO_Validation/CsvAppender.h"
#include "EL_FitsIO_Validation/ElBenchmark.h"

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variable_value;

using namespace Euclid::FitsIO;

Test::BenchmarkFactory initFactory() {
  Test::BenchmarkFactory factory;
  factory.registerBenchmark<Test::CfitsioBenchmark>("CFitsIO_rowwise", 1);
  factory.registerBenchmark<Test::CfitsioBenchmark>("CFitsIO_colwise", -1);
  factory.registerBenchmark<Test::CfitsioBenchmark>("CFitsIO", 0);
  factory.registerBenchmark<Test::ElColwiseBenchmark>("EL_FitsIO_colwise");
  factory.registerBenchmark<Test::ElBenchmark>("EL_FitsIO");
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

class EL_FitsIO_WritePerf : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    add("setup", value<std::string>()->default_value("EL_FitsIO"), "Test setup to be benchmarked");
    add("images", value<int>()->default_value(0), "Number of image extensions");
    add("pixels", value<int>()->default_value(1), "Number of pixels");
    add("tables", value<int>()->default_value(0), "Number of binary table extensions");
    add("rows", value<int>()->default_value(1), "Number of rows");
    add("output", value<std::string>()->default_value("/tmp/test.fits"), "Output Fits file");
    add("res", value<std::string>()->default_value("/tmp/benchmark.csv"), "Output result file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_WritePerf");

    const auto testSetup = args["setup"].as<std::string>();
    const auto imageCount = args["images"].as<int>();
    const auto pixelCount = args["pixels"].as<int>();
    const auto tableCount = args["tables"].as<int>();
    const auto rowCount = args["rows"].as<int>();
    const auto filename = args["output"].as<std::string>();
    const auto results = args["res"].as<std::string>();

    logger.info("Setting up the benchmark...");

    auto benchmark = initFactory() // TODO pass factory to CTor of the program, with initFactory() as default
                         .createBenchmark(testSetup, filename);
    if (not benchmark) {
      throw Test::TestCaseNotImplemented(std::string("No setup named: ") + testSetup);
    }
    Test::CsvAppender writer(
        results,
        { "Date",
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
          "Samples (ms)" });

    if (imageCount) {

      logger.info("Generating raster...");

      const Test::BRaster raster = Test::RandomRaster<std::int64_t, 1>({ pixelCount });

      logger.info("Writing image HDUs...");

      try {
        const auto chrono = benchmark->writeImages(imageCount, raster);
        writer.writeRow(
            "TODO",
            testSetup,
            "Write",
            "Image",
            imageCount,
            pixelCount,
            imageCount * pixelCount,
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
        const auto chrono = benchmark->readImages(1, imageCount);
        writer.writeRow(
            "TODO",
            testSetup,
            "Read",
            "Image",
            imageCount,
            pixelCount,
            imageCount * pixelCount,
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

    } else if (tableCount) {

      logger.info("Generating columns...");

      const auto table = Test::RandomTable(1, rowCount);
      const Test::BColumns columns = std::make_tuple(
          std::move(table.getColumn<unsigned char>()),
          std::move(table.getColumn<std::int32_t>()),
          std::move(table.getColumn<std::int64_t>()),
          std::move(table.getColumn<float>()),
          std::move(table.getColumn<double>()),
          std::move(table.getColumn<std::complex<float>>()),
          std::move(table.getColumn<std::complex<double>>()),
          std::move(table.getColumn<char>()),
          std::move(table.getColumn<std::uint32_t>()),
          std::move(table.getColumn<std::uint64_t>()));

      logger.info("Writing binary table HDUs...");

      try {
        const auto chrono = benchmark->writeBintables(tableCount, columns);
        writer.writeRow(
            "TODO",
            testSetup,
            "Write",
            "Binary table",
            tableCount,
            rowCount * Test::columnCount,
            tableCount * rowCount * Test::columnCount,
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
        const auto chrono = benchmark->readBintables(1 + imageCount, tableCount);
        writer.writeRow(
            "TODO",
            testSetup,
            "Read",
            "Binary table",
            tableCount,
            rowCount * Test::columnCount,
            tableCount * rowCount * Test::columnCount,
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
      throw Test::TestCaseNotImplemented(
          "There should be either a positive number of image HDUs or a positive number of binary table HDUs");
    }

    logger.info("Done.");

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_WritePerf)
