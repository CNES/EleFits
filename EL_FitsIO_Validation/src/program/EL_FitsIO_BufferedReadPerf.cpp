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

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

#include "EL_FitsData/FitsDataFixture.h"
#include "EL_FitsFile/MefFile.h"

using namespace Euclid::FitsIO;

using value_type = float;
using column_type = VecColumn<value_type>;
using table_type = std::vector<column_type>;

table_type generate_table(long cols, long rows) {
  table_type table(cols);
  for (long c=0; c<cols; ++c) {
    auto data = Test::generateRandomVector<value_type>(rows);
    table[c] = column_type { { std::to_string(c), "", 1 }, std::move(data) };
  }
  return table;
}

class EL_FitsIO_BufferedReadPerf : public Elements::Program {

public:

  options_description defineSpecificProgramOptions() override {
    options_description options {};
    options.add_options()
        ("tables", value<int>()->default_value(1), "Number of bintable extensions")
        ("rows", value<int>()->default_value(1), "Number of rows")
        ("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_BufferedReadPerf");

    const auto count = args["tables"].as<int>();
    const auto rows = args["rows"].as<int>();
    const auto cols = 10;  // TODO
    const auto filename = args["output"].as<std::string>();


    MefFile f(filename, FitsFile::Permission::Overwrite);

    logger.info() << "Writing " << count << " bintable extension(s)"
        << " of " << cols << " columns and " << rows << " rows";

    {
      const auto table = generate_table(cols, rows);
      for(int i=0; i<count; ++i)
        f.assignBintableExt("T_" + std::to_string(i),
            table[0], table[1], table[2], table[3], table[4], table[5], table[6], table[7], table[8], table[9]);
    }

    logger.info() << "Reading column-wise";
    auto begin = std::chrono::steady_clock::now();
    for(int i=0; i<count; ++i) {
      const auto& ext = f.accessFirst<BintableHdu>("T_" + std::to_string(i));
      for(int j=0; j<cols; ++j)
        ext.readColumn<value_type>(std::to_string(j));
    }
    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    logger.info() << "\tElapsed: " << duration_ms << " ms";

    logger.info() << "Reading row-wise";
    begin = std::chrono::steady_clock::now();
    for(int i=0; i<count; ++i) {
      const auto& ext = f.accessFirst<BintableHdu>("T_" + std::to_string(i));
      std::vector<std::string> names(cols);
      for(int j=0; j<cols; ++j)
        names[j] = std::to_string(j);
      ext.readColumns<
        value_type, value_type, value_type, value_type, value_type,
        value_type, value_type, value_type, value_type, value_type>(names);
    }
    end = std::chrono::steady_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    logger.info() << "\tElapsed: " << duration_ms << " ms";

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(EL_FitsIO_BufferedReadPerf)
