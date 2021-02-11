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
  enum Setup { Cfitsio = 0, ElUnbuffered, El };
  static const std::vector<std::string> names;
  static Test::Benchmark* create(Setup setup, const std::string& filename) {
    switch (setup) {
      case Cfitsio:
        return new Test::CfitsioBenchmark(filename);
      case ElUnbuffered:
        return new Test::ElUnbufferedBenchmark(filename);
      case El:
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

const std::vector<std::string> BenchmarkFactory::names = { "CFitsIO", "EL_FitsIO_v1", "EL_FitsIO" };

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
    Test::CsvAppender writer(
        results,
        { "Date",
          "Test setup",
          "Mode",
          "HDU type",
          "HDU count",
          "Value count / HDU",
          "Total value count",
          "Elapsed (ms)",
          "Mean (ms)",
          "Standard deviation (ms)" });

    if (imageCount) {

      logger.info("Generating raster...");

      const Test::BRaster raster = Test::RandomRaster<std::int64_t, 1>({ pixelCount });

      logger.info("Writing image HDUs...");

      const auto imageChrono = benchmark->writeImages(imageCount, raster);
      writer.writeRow(
          "TODO",
          testSetup,
          "Write",
          "Image",
          imageCount,
          pixelCount,
          imageCount * pixelCount,
          imageChrono.elapsed().count(),
          imageChrono.mean(),
          imageChrono.stdev());
      // TODO read
    }

    if (tableCount) {

      logger.info("Generating columns...");

      const auto table = Test::RandomTable(1, rowCount);
      const Test::BColumns columns = std::make_tuple(
          std::cref(table.getColumn<unsigned char>()),
          std::cref(table.getColumn<std::int32_t>()),
          std::cref(table.getColumn<std::int64_t>()),
          std::cref(table.getColumn<float>()),
          std::cref(table.getColumn<double>()),
          std::cref(table.getColumn<std::complex<float>>()),
          std::cref(table.getColumn<std::complex<double>>()),
          std::cref(table.getColumn<char>()),
          std::cref(table.getColumn<std::uint32_t>()),
          std::cref(table.getColumn<std::uint64_t>()));

      logger.info("Writing binary table HDUs...");

      const auto tableChrono = benchmark->writeBintables(tableCount, columns);
      writer.writeRow(
          "TODO",
          testSetup,
          "Write",
          "Binary table",
          tableCount,
          rowCount * Test::columnCount,
          tableCount * rowCount * Test::columnCount,
          tableChrono.elapsed().count(),
          tableChrono.mean(),
          tableChrono.stdev());
      // TODO read
    }

    logger.info("Done.");

    delete benchmark;
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_WritePerf)
