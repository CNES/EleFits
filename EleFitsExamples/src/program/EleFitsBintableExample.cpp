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
const Fits::BintableHdu& write_bintable(Fits::MefFile& f, const std::string& extName, long rows) {

  /* A string column */
  auto stringInfo = Fits::makeColumnInfo<std::string>("STRING", "", 6);
  auto stringData = Fits::Test::generate_random_vector<std::string>(rows);
  auto stringCol = makeColumn(stringInfo, std::move(stringData));

  /* A scalar column of complex values */
  auto scalarInfo = Fits::makeColumnInfo<std::complex<float>>("SCALAR");
  auto scalarData = Fits::Test::generate_random_vector<std::complex<float>>(rows);
  auto scalarCol = makeColumn(scalarInfo, std::move(scalarData));

  /* A vector column of int16 values */
  auto vectorInfo = Fits::makeColumnInfo<std::int16_t>("VECTOR", "", 3);
  auto vectorData = Fits::Test::generate_random_vector<std::int16_t>(rows * vectorInfo.repeatCount());
  auto vectorCol = makeColumn(vectorInfo, std::move(vectorData));

  /* A multidimensional column of uint16 values */
  auto multidimInfo = Fits::makeColumnInfo<std::uint16_t>("MULTIDIM", "", 6, 4);
  auto multidimData = Fits::Test::generate_random_vector<std::uint16_t>(rows * multidimInfo.repeatCount());
  auto multidimCol = makeColumn(multidimInfo, std::move(multidimData));

  /* Create the table */
  return f.appendBintable(extName, {}, stringCol, scalarCol, vectorCol, multidimCol);
}

/*
 * Append a column to an existing table.
 * 
 * This function also shows how to work with standalone data pointers instead of `Column` objects,
 * thanks to the `makeColumn()` builder function.
 * It is also possible to append or insert several columns at once, analogously to what's done in `write_bintable()`.
 */
template <typename TInfo, typename T>
void append_column(const Fits::BintableColumns& du, const TInfo& info, const T* data) {
  const auto rows = du.readRowCount();
  du.init(info);
  du.write(makeColumn(info, rows, data));
}

/**
 * Read columns.
 * 
 * Like when writing, reading several columns at once is much faster than reading them one by one.
 * For more fun, let's compute something from all the elements of several columns.
 */
double readColumns(const Fits::BintableColumns& du) {

  /* Read with different types (implicit conversion) */
  const auto cols = du.readSeq(Fits::as<double>("VECTOR"), Fits::as<float>("MULTIDIM"));

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
 * Manipulate the entries of a multidimensional column as nD-arrays.
 */
void viewAsRaster(const Fits::BintableColumns& du) {

  /* Read a column of 2D entries */
  auto col = du.read<std::uint16_t, 2>("MULTIDIM");

  /* Zero pixels at odd positions */
  for (long i = 0; i < col.rowCount(); ++i) {
    auto raster = col.entry(i);
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
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Fits::ProgramOptions options("Generate, write and read a binary table.");
    options.positional("output", value<std::string>()->default_value("/tmp/bintable.fits"), "Output file");
    options.named("rows", value<long>()->default_value(42), "Number of rows");
    return options.asPair();
  }

  /*
   * Execute the program.
   */
  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Logging logger = Logging::getLogger("EleFitsBintableExample");
    const auto filename = args["output"].as<std::string>();
    const auto rows = args["rows"].as<long>();

    logger.info("Opening or creating the file...");
    Fits::MefFile f(filename, Fits::FileMode::Write);

    logger.info("Creating a binary table HDU...");
    const auto extName = "TABLE" + std::to_string(f.hduCount());
    const auto& bintable = write_bintable(f, extName, rows);

    logger.info("Appending a column...");
    Fits::ColumnInfo<double> info("BACK", "unit");
    const auto vec = Fits::Test::generate_random_vector<double>(rows, -1, 1);
    append_column(bintable.columns(), info, vec.data());

    logger.info("Reading columns...");
    auto result = readColumns(bintable.columns());
    logger.info() << "  Result = " << result;

    logger.info("Modifying a multidimensional column...");
    viewAsRaster(bintable.columns());

    logger.info("Done.");
    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsBintableExample)
