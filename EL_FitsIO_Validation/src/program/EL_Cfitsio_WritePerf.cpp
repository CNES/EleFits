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
#include <vector>

#include <boost/program_options.hpp>
#include <fitsio.h>

#include "ElementsKernel/ProgramHeaders.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

using Euclid::Cfitsio::c_str_array;

std::vector<float> generate_raster(long naxis1, long naxis2) {
  long order = 10;
  while (order < naxis2)
    order *= 10;
  std::vector<float> raster(naxis1 * naxis2);
  for (long j=0; j < naxis2; ++j) for (long i=0; i < naxis1; ++i)
    raster[i + j * naxis1] = float(i + j) / float(order);
  return raster;
}

struct colinfo_t {
  std::vector<char*> names;
  std::vector<char*> formats;
  std::vector<char*> units;
};

colinfo_t generate_colinfo() {
  std::vector<char*> names = { const_cast<char *>("STRINGS"), const_cast<char *>("FLOATS"), const_cast<char *>("INTS") };
  std::vector<char*> formats = { const_cast<char *>("8A"), const_cast<char *>("1E"), const_cast<char *>("1J") };
  std::vector<char*> units = { const_cast<char *>(""), const_cast<char *>(""), const_cast<char *>("") };
  return colinfo_t { std::move(names), std::move(formats), std::move(units) };
}

struct table_t {
  std::vector<std::string> strings;
  std::vector<float> floats;
  std::vector<int> ints;
};

table_t generate_columns(long naxis2) {
  std::vector<std::string> strings(naxis2);
  std::vector<float> floats(naxis2);
  std::vector<int> ints(naxis2);
  for(long i=0; i<naxis2; ++i) {
    strings[i] = "Text";
    floats[i] = float(i) / float(naxis2);
    ints[i] = int(i * naxis2);
  }
  table_t table;
  return table_t { std::move(strings), std::move(floats), std::move(ints) };
}

void create_image_ext(fitsfile* fptr, std::string extname, long* naxes, float* data) {
  int status = 0;
  int nhdu = 0;
  fits_get_num_hdus(fptr, &nhdu, &status);
  fits_create_img(fptr, FLOAT_IMG, 2, naxes, &status);
  fits_write_key(fptr, TSTRING, "EXTNAME", &extname[0], nullptr, &status);
  fits_write_img(fptr, TFLOAT, 1, naxes[0] * naxes[1], data, &status);
}

void create_table_ext(fitsfile* fptr, std::string extname, colinfo_t& colinfo, table_t& table) {
  int status = 0;
  long naxis2 = static_cast<long>(table.strings.size());
  fits_create_tbl(fptr, BINARY_TBL, 0, 3,
      colinfo.names.data(), colinfo.formats.data(), colinfo.units.data(),
      extname.c_str(), &status);
  fits_write_col(fptr, TSTRING, 1, 1, 1, naxis2, table.strings.data(), &status);
  fits_write_col(fptr, TFLOAT, 2, 1, 1, naxis2, table.floats.data(), &status);
  fits_write_col(fptr, TINT, 3, 1, 1, naxis2, table.ints.data(), &status);
}

class EL_Cfitsio_WritePerf : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
  
    options_description options {};
    options.add_options()
        ("images", value<int>()->default_value(0), "Number of image extensions")
        ("tables", value<int>()->default_value(0), "Number of bintable extensions")
        ("naxis1", value<int>()->default_value(1), "First axis size")
        ("naxis2", value<int>()->default_value(1), "Second axis size")
        ("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_Cfitsio_WritePerf");

    const auto image_count = args["images"].as<int>();
    const auto table_count = args["tables"].as<int>();
    const auto naxis1 = args["naxis1"].as<int>();
    const auto naxis2 = args["naxis2"].as<int>();
    const auto filename = args["output"].as<std::string>();

    long naxes[] = {naxis1, naxis2};
    auto raster = generate_raster(naxis1, naxis2); // Not const to avoid copies
    auto colinfo = generate_colinfo();
    auto table = generate_columns(naxis2);

    logger.info() << "Creating Fits file: " << filename;
    
    int status = 0;
    fitsfile *fptr;
    fits_create_file(&fptr, (std::string("!") + filename).c_str(), &status);
    long naxis0 = 0;
    fits_create_img(fptr, BYTE_IMG, 1, &naxis0, &status);

    logger.info() << "Generating " << image_count << " image extension(s)"
        << " of size " << naxis1 << " x " << naxis2;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for(int i=0; i<image_count; ++i)
      create_image_ext(fptr, "I_" + std::to_string(i), naxes, raster.data());
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    logger.info() << "\tElapsed: " << duration_ms << " ms";

    logger.info() << "Generating " << table_count << " bintable extension(s)"
        << " of size " << 3 << " x " << naxis2;

    begin = std::chrono::steady_clock::now();
    for(int i=0; i<table_count; ++i)
      create_table_ext(fptr, "T_" + std::to_string(i), colinfo, table);
    end = std::chrono::steady_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    logger.info() << "\tElapsed: " << duration_ms << " ms";

    fits_close_file(fptr, &status);

    return status==0 ? Elements::ExitCode::OK : Elements::ExitCode::NOT_OK;
  }

};

MAIN_FOR(EL_Cfitsio_WritePerf)
