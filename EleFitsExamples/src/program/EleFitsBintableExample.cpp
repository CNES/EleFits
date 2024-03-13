// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsData/TestUtils.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

using boost::program_options::value;

using namespace Euclid;

/*
 * Create a binary table extension.
 * 
 * A set of columns of various types and shapes is first randmoly generated,
 * and a new extension is appended to the file, where the columns are all written in one call.
 * When several columns are written at once, the operation is optimized through an internal buffer,
 * which makes it much faster than writing columns one by one.
 */
const Fits::BintableHdu& write_bintable(Fits::MefFile& f, const std::string& ext_name, Linx::Index rows)
{
  /* A string column */
  auto string_info = Fits::make_column_info<std::string>("STRING", "", 6);
  auto string_data = Fits::Test::generate_random_vector<std::string>(rows);
  auto string_col = make_column(string_info, std::move(string_data));

  /* A scalar column of complex values */
  auto scalar_info = Fits::make_column_info<std::complex<float>>("SCALAR");
  auto scalar_data = Fits::Test::generate_random_vector<std::complex<float>>(rows);
  auto scalar_col = make_column(scalar_info, std::move(scalar_data));

  /* A vector column of int16 values */
  auto vector_info = Fits::make_column_info<std::int16_t>("VECTOR", "", 3);
  auto vector_data = Fits::Test::generate_random_vector<std::int16_t>(rows * vector_info.repeat_count());
  auto vector_col = make_column(vector_info, std::move(vector_data));

  /* A multidimensional column of uint16 values */
  auto multidim_info = Fits::make_column_info<std::uint16_t>("MULTIDIM", "", 6, 4);
  auto multidim_data = Fits::Test::generate_random_vector<std::uint16_t>(rows * multidim_info.repeat_count());
  auto multidim_col = make_column(multidim_info, std::move(multidim_data));

  /* Create the table */
  return f.append_bintable(ext_name, {}, string_col, scalar_col, vector_col, multidim_col);
}

/*
 * Append a column to an existing table.
 * 
 * This function also shows how to work with standalone data pointers instead of `Column` objects,
 * thanks to the `make_column()` builder function.
 * It is also possible to append or insert several columns at once, analogously to what's done in `write_bintable()`.
 */
template <typename TInfo, typename T>
void append_column(const Fits::BintableColumns& du, const TInfo& info, const T* data)
{
  const auto rows = du.read_row_count();
  du.init(info);
  du.write(make_column(info, rows, data));
}

/**
 * Read columns.
 * 
 * Like when writing, reading several columns at once is much faster than reading them one by one.
 * For more fun, let's compute something from all the elements of several columns.
 */
double read_columns(const Fits::BintableColumns& du)
{
  /* Read with different types (implicit conversion) */
  const auto cols = du.read_n(Fits::as<double>("VECTOR"), Fits::as<float>("MULTIDIM"));

  /* Compute something */
  double result = 0;
  for (const auto& a : std::get<0>(cols)) {
    result += a;
  }
  for (const auto& b : std::get<1>(cols)) {
    result -= b;
  }

  return result;
}

/*
 * Manipulate the fields of a multidimensional column as nD-arrays.
 */
void view_as_raster(const Fits::BintableColumns& du)
{
  /* Read a column of 2D fields */
  auto col = du.read<std::uint16_t, 2>("MULTIDIM");

  /* Zero pixels at odd positions */
  for (Linx::Index i = 0; i < col.row_count(); ++i) {
    auto raster = col.field(i);
    for (const auto& p : raster.domain()) {
      if ((p[0] + p[1]) % 2 == 1) {
        raster[p] = 0;
      }
    }
  }

  /* Overwrite the column */
  du.write(col);
}

/*
 * The progam.
 */
class EleFitsBintableExample : public Elements::Program {
public:

  /*
   * Declare the program options.
   */
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override
  {
    Fits::ProgramOptions options("Generate, write and read a binary table.");
    options.positional("output", value<std::string>()->default_value("/tmp/bintable.fits"), "Output file");
    options.named("rows", value<Linx::Index>()->default_value(42), "Number of rows");
    return options.as_pair();
  }

  /*
   * Execute the program.
   */
  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override
  {
    Logging logger = Logging::getLogger("EleFitsBintableExample");
    const auto filename = args["output"].as<std::string>();
    const auto rows = args["rows"].as<Linx::Index>();

    logger.info("Opening or creating the file...");
    Fits::MefFile f(filename, Fits::FileMode::Write);

    logger.info("Creating a binary table HDU...");
    const auto ext_name = "TABLE" + std::to_string(f.hdu_count());
    const auto& bintable = write_bintable(f, ext_name, rows);

    logger.info("Appending a column...");
    Fits::ColumnInfo<double> info("BACK", "unit");
    const auto vec = Fits::Test::generate_random_vector<double>(rows, -1, 1);
    append_column(bintable.columns(), info, vec.data());

    logger.info("Reading columns...");
    auto result = read_columns(bintable.columns());
    logger.info() << "  Result = " << result;

    logger.info("Modifying a multidimensional column...");
    view_as_raster(bintable.columns());

    logger.info("Done.");
    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsBintableExample)
