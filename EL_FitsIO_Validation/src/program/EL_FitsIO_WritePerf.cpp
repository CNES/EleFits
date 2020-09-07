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

#include "EL_FitsFile/MefFile.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using namespace Euclid::FitsIO;

VecRaster<float> generateRaster(long naxis1, long naxis2) {
  long order = 10;
  while (order < naxis2) {
    order *= 10;
  }
  VecRaster<float, 2> raster({ naxis1, naxis2 });
  for (long j = 0; j < naxis2; ++j) {
    for (long i = 0; i < naxis1; ++i) {
      raster[{ i, j }] = float(i + j) / float(order);
    }
  }
  return raster;
}

struct Table {
  VecColumn<std::string> stringCol;
  VecColumn<float> floatCol;
  VecColumn<int> intCol;
};

Table generateColumns(long naxis2) {
  std::vector<std::string> strings(naxis2);
  std::vector<float> floats(naxis2);
  std::vector<int> ints(naxis2);
  for (long i = 0; i < naxis2; ++i) {
    strings[i] = "Text";
    floats[i] = float(i) / float(naxis2);
    ints[i] = int(i * naxis2);
  }
  Table table { VecColumn<std::string>({ "STRINGS", "", 8 }, std::move(strings)),
                VecColumn<float>({ "FLOATS", "", 1 }, std::move(floats)),
                VecColumn<int>({ "INTS", "", 1 }, std::move(ints)) };
  return table;
}

class EL_FitsIO_WritePerf : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    add("images", value<int>()->default_value(0), "Number of image extensions");
    add("tables", value<int>()->default_value(0), "Number of bintable extensions");
    add("naxis1", value<int>()->default_value(1), "First axis size");
    add("naxis2", value<int>()->default_value(1), "Second axis size");
    add("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_WritePerf");

    const auto imageCount = args["images"].as<int>();
    const auto tableCount = args["tables"].as<int>();
    const auto naxis1 = args["naxis1"].as<int>();
    const auto naxis2 = args["naxis2"].as<int>();
    const auto filename = args["output"].as<std::string>();

    const auto raster = generateRaster(naxis1, naxis2);
    const auto table = generateColumns(naxis2);

    MefFile f(filename, FitsFile::Permission::Overwrite);

    logger.info() << "Generating " << imageCount << " image extension(s)"
                  << " of size " << naxis1 << " x " << naxis2;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int i = 0; i < imageCount; ++i) {
      f.assignImageExt("I_" + std::to_string(i), raster);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto durationMilli = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    logger.info() << "\tElapsed: " << durationMilli << " ms";

    logger.info() << "Generating " << tableCount << " bintable extension(s)"
                  << " of size " << 3 << " x " << naxis2;

    begin = std::chrono::steady_clock::now();
    for (int i = 0; i < tableCount; ++i) {
      f.assignBintableExt("T_" + std::to_string(i), table.stringCol, table.floatCol, table.intCol);
    }
    end = std::chrono::steady_clock::now();
    durationMilli = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    logger.info() << "\tElapsed: " << durationMilli << " ms";

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EL_FitsIO_WritePerf)
