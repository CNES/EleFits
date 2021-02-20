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

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsData/TestColumn.h"

#include "EL_FitsIO_Validation/CfitsioBenchmark.h"
#include "EL_FitsIO_Validation/CsvAppender.h"
#include "EL_FitsIO_Validation/ElBenchmark.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid::FitsIO;

struct BenchmarkFactory {
  enum Setup { CfitsioColwise, CfitsioBuffered, CfitsioRowwise, ElColwise, ElBuffered };
  static const std::vector<std::string> names;
  static Test::Benchmark* create(Setup setup, const std::string& filename) {
    switch (setup) {
      case CfitsioColwise:
        return new Test::CfitsioBenchmark(filename, -1);
      case CfitsioBuffered:
        return new Test::CfitsioBenchmark(filename, 0);
      case CfitsioRowwise:
        return new Test::CfitsioBenchmark(filename, 1);
      case ElColwise:
        return new Test::ElColwiseBenchmark(filename);
      case ElBuffered:
        return new Test::ElBenchmark(filename);
      default:
        return nullptr;
    }
  }
  static Test::Benchmark* create(const std::string& name, const std::string& filename) {
    const auto begin = names.begin();
    const auto end = names.end();
    const auto it = std::find(begin, end, name);
    const auto setup = static_cast<Setup>(std::distance(begin, it));
    return create(setup, filename);
  }
};

const std::vector<std::string> BenchmarkFactory::names = { "CFitsIO",
                                                           "CFitsIO_buffered",
                                                           "CFitsIO_rowwise",
                                                           "EL_FitsIO_colwise",
                                                           "EL_FitsIO" };

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

    Test::Benchmark* benchmark = BenchmarkFactory::create(testSetup, filename);
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
          "Standard deviation (ms)" });

    if (imageCount) {

      logger.info("Generating raster...");

      const Test::BRaster raster = Test::RandomRaster<std::int64_t, 1>({ pixelCount });

      logger.info("Writing image HDUs...");

      try {
        const auto writeChrono = benchmark->writeImages(imageCount, raster);
        writer.writeRow(
            "TODO",
            testSetup,
            "Write",
            "Image",
            imageCount,
            pixelCount,
            imageCount * pixelCount,
            boost::filesystem::file_size(filename),
            writeChrono.elapsed().count(),
            writeChrono.min(),
            writeChrono.max(),
            writeChrono.mean(),
            writeChrono.stdev());
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

      logger.info("Reading image HDUs...");

      try {
        const auto readChrono = benchmark->readImages(1, imageCount);

        writer.writeRow(
            "TODO",
            testSetup,
            "Read",
            "Image",
            imageCount,
            pixelCount,
            imageCount * pixelCount,
            boost::filesystem::file_size(filename),
            readChrono.elapsed().count(),
            readChrono.min(),
            readChrono.max(),
            readChrono.mean(),
            readChrono.stdev());
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
        const auto writeChrono = benchmark->writeBintables(tableCount, columns);
        writer.writeRow(
            "TODO",
            testSetup,
            "Write",
            "Binary table",
            tableCount,
            rowCount * Test::columnCount,
            tableCount * rowCount * Test::columnCount,
            boost::filesystem::file_size(filename),
            writeChrono.elapsed().count(),
            writeChrono.min(),
            writeChrono.max(),
            writeChrono.mean(),
            writeChrono.stdev());
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

      logger.info("Reading binary table HDUs...");

      try {
        const auto readChrono = benchmark->readBintables(1 + imageCount, tableCount);

        writer.writeRow(
            "TODO",
            testSetup,
            "Read",
            "Binary table",
            tableCount,
            rowCount * Test::columnCount,
            tableCount * rowCount * Test::columnCount,
            boost::filesystem::file_size(filename),
            readChrono.elapsed().count(),
            readChrono.min(),
            readChrono.max(),
            readChrono.mean(),
            readChrono.stdev());
      } catch (const std::exception& e) {
        logger.warn() << e.what();
      }

    } else {
      throw Test::TestCaseNotImplemented(
          "There should be either a positive number of image HDUs or a positive number of binary table HDUs");
    }

    logger.info("Done.");

    delete benchmark;

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_WritePerf)
