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
#include <fitsio.h>

#include "ElementsKernel/ProgramHeaders.h"

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsData/TestColumn.h"
#include "EL_FitsFile/MefFile.h"
#include "EL_FitsIO_Validation/Chronometer.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid;
using Raster = FitsIO::Raster<std::int64_t, 1>;
using Columns = std::tuple<
    FitsIO::VecColumn<char>,
    FitsIO::VecColumn<std::int32_t>,
    FitsIO::VecColumn<float>,
    FitsIO::VecColumn<std::complex<double>>,
    FitsIO::VecColumn<std::string>>;
using Chronometer = FitsIO::Test::Chronometer<std::chrono::milliseconds>;

class Benchmark {
public:
  virtual ~Benchmark() = default;

  Benchmark() : m_chrono(), m_logger(Elements::Logging::getLogger("Benchmark")) {
  }

  std::vector<Chronometer::Unit> writeImages(int count, const Raster& raster) {
    m_chrono.reset();
    std::vector<Chronometer::Unit> results(count + 1);
    for (int i = 0; i < count; ++i) {
      const auto inc = writeImage(raster);
      m_logger.info() << i + 1 << "/" << count << ": " << inc.count() << "ms";
      results[i] = inc;
    }
    const auto total = m_chrono.elapsed();
    m_logger.info() << "TOTAL: " << total.count() << "ms";
    results[count] = total;
    return results;
  }

  std::vector<Chronometer::Unit> writeBintables(int count, const Columns& columns) { // TODO avoid duplication
    m_chrono.reset();
    std::vector<Chronometer::Unit> results(count + 1);
    for (int i = 0; i < count; ++i) {
      const auto inc = writeBintable(columns);
      m_logger.info() << i + 1 << "/" << count << ": " << inc.count() << "ms";
      results[i] = inc;
    }
    const auto total = m_chrono.elapsed();
    m_logger.info() << "TOTAL: " << total.count() << "ms";
    results[count] = total;
    return results;
  }

  virtual Chronometer::Unit writeImage(const Raster& raster) = 0;

  virtual Chronometer::Unit writeBintable(const Columns& columns) = 0;

protected:
  Chronometer m_chrono;
  Elements::Logging m_logger;
};

class ElfitsioBenchmark : public Benchmark {
public:
  virtual ~ElfitsioBenchmark() = default;

  ElfitsioBenchmark(const std::string& filename) : Benchmark(), m_f(filename, FitsIO::MefFile::Permission::Overwrite) {
  }

  virtual Chronometer::Unit writeImage(const Raster& raster) override {
    m_chrono.start();
    m_f.assignImageExt("", raster);
    return m_chrono.stop();
  }

  virtual Chronometer::Unit writeBintable(const Columns& columns) override {
    m_chrono.start();
    m_f.assignBintableExt("", columns);
    return m_chrono.stop();
  }

private:
  FitsIO::MefFile m_f;
};

class CfitsioBenchmark : public Benchmark {
public:
  virtual ~CfitsioBenchmark() = default;

  CfitsioBenchmark(const std::string& filename) : Benchmark(), m_fptr(nullptr), m_status(0) {
    fits_create_file(&m_fptr, filename.c_str(), &m_status);
  }

  virtual Chronometer::Unit writeImage(const Raster& raster) override {
    m_chrono.start();
    auto nonconstShape = raster.shape;
    fits_create_img(
        m_fptr,
        Cfitsio::TypeCode<Raster::Value>::bitpix(),
        raster.shape.size(),
        nonconstShape.data(),
        &m_status);
    std::vector<Raster::Value> nonconstData(raster.data(), raster.data() + raster.size());
    fits_write_img(
        m_fptr,
        Cfitsio::TypeCode<Raster::Value>::forImage(),
        1,
        raster.size(),
        nonconstData.data(),
        &m_status);
    Cfitsio::CfitsioError::mayThrow(m_status);
    return m_chrono.stop();
  }

  virtual Chronometer::Unit writeBintable(const Columns& columns) override {
    m_chrono.start();
    // FIXME
    return m_chrono.stop();
  }

private:
  fitsfile* m_fptr;
  int m_status;
};

class EL_FitsIO_WritePerf : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    add("test", value<std::string>()->default_value("EL_FitsIO"), "Test case to be benchmarked (CFitsIO or EL_FitsIO)");
    add("images", value<int>()->default_value(0), "Number of image extensions");
    add("pixels", value<int>()->default_value(1), "Number of pixels");
    add("tables", value<int>()->default_value(0), "Number of binary table extensions");
    add("rows", value<int>()->default_value(1), "Number of rows");
    add("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_WritePerf");

    const auto testCase = args["test"].as<std::string>();
    const auto imageCount = args["images"].as<int>();
    const auto pixels = args["pixels"].as<int>();
    const auto tableCount = args["tables"].as<int>();
    const auto rows = args["rows"].as<int>();
    const auto filename = args["output"].as<std::string>();

    const auto raster = FitsIO::Test::RandomRaster<std::int64_t, 1>({ pixels });
    const auto table = FitsIO::Test::RandomTable(rows);
    const auto columns = std::make_tuple(
        table.getColumn<char>(),
        table.getColumn<std::int32_t>(),
        table.getColumn<float>(),
        table.getColumn<std::complex<double>>(),
        table.getColumn<std::string>());

    Benchmark* benchmark = nullptr;
    if (testCase == "EL_FitsIO") {
      benchmark = new ElfitsioBenchmark(filename);
    } else if (testCase == "CFitsIO") {
      benchmark = new CfitsioBenchmark(filename);
    }
    benchmark->writeImages(imageCount, raster);
    benchmark->writeBintables(tableCount, columns);
    delete benchmark;
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_WritePerf)
