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
    const FitsIO::VecColumn<unsigned char>&,
    const FitsIO::VecColumn<std::int32_t>&,
    const FitsIO::VecColumn<std::int64_t>&,
    const FitsIO::VecColumn<float>&,
    const FitsIO::VecColumn<double>&,
    const FitsIO::VecColumn<std::complex<float>>&,
    const FitsIO::VecColumn<std::complex<double>>&,
    // const FitsIO::VecColumn<std::string>&, //TODO to be handled specially
    const FitsIO::VecColumn<char>&,
    const FitsIO::VecColumn<std::uint32_t>&,
    const FitsIO::VecColumn<std::uint64_t>&>;
constexpr std::size_t columnCount = std::tuple_size<Columns>::value;
using Chronometer = FitsIO::Test::Chronometer<std::chrono::milliseconds>;

/**
 * @brief A CSV writer which creates a new file or appends rows to an existing file.
 */
class CsvAppender {
public:
  /**
   * @brief Constructor.
   * @param header The file header
   * @param sep The column separator
   * @details
   * If the file exists and the header is provided, it should match the first row of the file.
   * If the file does not exist, the header is written.
   */
  CsvAppender(const std::string& filename, const std::vector<std::string>& header = {}, const std::string& sep = "\t") :
      m_file(filename, std::ios::out | std::ios::app),
      m_sep(sep) {
    if (header.empty()) {
      return;
    }
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    const bool append = in.tellg();
    if (append) {
      // TODO check header consistency
    } else {
      for (const auto h : header) {
        (*this) << h;
      }
      (*this) << std::endl;
    }
  };

  /**
   * @brief Write a value.
   */
  template <typename T>
  CsvAppender& operator<<(const T& value) {
    m_file << value << m_sep;
    return *this;
  }

  /**
   * @brief Apply a manipulator, e.g. `std::endl`.
   */
  CsvAppender& operator<<(std::ostream& (*pf)(std::ostream&)) {
    m_file << pf;
    return *this;
  }

  /**
   * @brief Write a row.
   */
  template <typename... Ts>
  CsvAppender& writeRow(const Ts&... values) {
    // TODO check size
    using mockUnpack = int[];
    (void)mockUnpack { (operator<<(values), 0)... };
    return operator<<(std::endl);
  }

private:
  /** @brief The output stream. */
  std::ofstream m_file;

  /** @brief The column separator. */
  std::string m_sep;
};

class Benchmark {
public:
  virtual ~Benchmark() = default;

  Benchmark() : m_chrono(), m_logger(Elements::Logging::getLogger("Benchmark")) {
  }

  const Chronometer& writeImages(int count, const Raster& raster) {
    m_chrono.reset();
    for (int i = 0; i < count; ++i) {
      const auto inc = writeImage(raster);
      m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
    }
    const auto total = m_chrono.elapsed();
    m_logger.debug() << "TOTAL: " << total.count() << "ms";
    return m_chrono;
  }

  const Chronometer& writeBintables(int count, const Columns& columns) { // TODO avoid duplication
    m_chrono.reset();
    for (int i = 0; i < count; ++i) {
      const auto inc = writeBintable(columns);
      m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
    }
    const auto total = m_chrono.elapsed();
    m_logger.debug() << "TOTAL: " << total.count() << "ms";
    return m_chrono;
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
    long rowCount = std::get<0>(columns).rowCount();
    std::vector<std::string> names(rowCount);
    std::vector<std::string> formats(rowCount);
    std::vector<std::string> units(rowCount);
    setupColumnInfo<0>(columns, names, formats, units);
    setupColumnInfo<1>(columns, names, formats, units);
    setupColumnInfo<2>(columns, names, formats, units);
    setupColumnInfo<3>(columns, names, formats, units);
    setupColumnInfo<4>(columns, names, formats, units);
    setupColumnInfo<5>(columns, names, formats, units);
    setupColumnInfo<6>(columns, names, formats, units);
    setupColumnInfo<7>(columns, names, formats, units);
    setupColumnInfo<8>(columns, names, formats, units);
    setupColumnInfo<9>(columns, names, formats, units); // TODO index_sequence
    Cfitsio::CStrArray nameArray(names);
    Cfitsio::CStrArray formatArray(formats);
    Cfitsio::CStrArray unitArray(units);
    m_chrono.start();
    fits_create_tbl(
        m_fptr,
        BINARY_TBL,
        0,
        columnCount,
        nameArray.data(),
        formatArray.data(),
        unitArray.data(),
        "",
        &m_status);
    Cfitsio::CfitsioError::mayThrow(m_status);
    writeColumn<0>(columns, rowCount);
    writeColumn<1>(columns, rowCount);
    writeColumn<2>(columns, rowCount);
    writeColumn<3>(columns, rowCount);
    writeColumn<4>(columns, rowCount);
    writeColumn<5>(columns, rowCount);
    writeColumn<6>(columns, rowCount);
    writeColumn<7>(columns, rowCount);
    writeColumn<8>(columns, rowCount);
    writeColumn<9>(columns, rowCount); // TODO index_sequence
    return m_chrono.stop();
  }

  template <std::size_t i>
  void setupColumnInfo(
      const Columns& columns,
      std::vector<std::string>& names,
      std::vector<std::string>& formats,
      std::vector<std::string> units) {
    const auto& col = std::get<i>(columns);
    names[i] = col.info.name;
    formats[i] = Cfitsio::TypeCode<typename std::decay_t<decltype(col)>::Value>::tform(col.info.repeatCount);
    units[i] = col.info.unit;
  }

  template <std::size_t i>
  void writeColumn(const Columns& columns, long rowCount) {
    const auto& col = std::get<i>(columns);
    auto nonconstVec = col.vector();
    fits_write_col(
        m_fptr,
        Cfitsio::TypeCode<typename std::decay_t<decltype(col)>::Value>::forBintable(),
        i + 1,
        1,
        1,
        rowCount,
        nonconstVec.data(),
        &m_status);
    Cfitsio::CfitsioError::mayThrow(m_status);
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
    add("output", value<std::string>()->default_value("/tmp/test.fits"), "Output Fits file");
    add("res", value<std::string>()->default_value("/tmp/benchmark.csv"), "Output result file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_WritePerf");

    const auto testCase = args["test"].as<std::string>();
    const auto imageCount = args["images"].as<int>();
    const auto pixelCount = args["pixels"].as<int>();
    const auto tableCount = args["tables"].as<int>();
    const auto rowCount = args["rows"].as<int>();
    const auto filename = args["output"].as<std::string>();
    const auto results = args["res"].as<std::string>();

    const auto raster = FitsIO::Test::RandomRaster<std::int64_t, 1>({ pixelCount });
    const auto table = FitsIO::Test::RandomTable(1, rowCount);
    const Columns columns = std::make_tuple(
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

    Benchmark* benchmark = nullptr;
    if (testCase == "EL_FitsIO") {
      benchmark = new ElfitsioBenchmark(filename);
    } else if (testCase == "CFitsIO") {
      benchmark = new CfitsioBenchmark(filename);
    }
    CsvAppender writer(
        results,
        { "Test case",
          "HDU type",
          "HDU count",
          "Value count / HDU",
          "Total value count",
          "Elapsed (ms)",
          "Mean (ms)",
          "Standard deviation (ms)" });
    if (imageCount) {
      const auto imageChrono = benchmark->writeImages(imageCount, raster);
      writer.writeRow(
          testCase,
          "Image",
          imageCount,
          pixelCount,
          imageCount * pixelCount,
          imageChrono.elapsed().count(),
          imageChrono.mean(),
          imageChrono.stdev());
    }
    if (tableCount) {
      const auto tableChrono = benchmark->writeBintables(tableCount, columns);
      writer.writeRow(
          testCase,
          "Binary table",
          tableCount,
          rowCount * columnCount,
          tableCount * rowCount * columnCount,
          tableChrono.elapsed().count(),
          tableChrono.mean(),
          tableChrono.stdev());
    }
    delete benchmark;
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_WritePerf)
