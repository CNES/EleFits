// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

/**
 * @example EleFitsTutorial.cpp
 * @brief Basic usage of the main services of EleFits.
 */

#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

//! [Include fixtures]
#include "EleFitsData/TestColumn.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsData/TestRecord.h"
//! [Include fixtures]

//! [Include]
#include "EleFits/MefFile.h"

using namespace Euclid;
// EleFits API is in the Euclid::Fits namespace.
// We could have be using namespace Euclid::Fits instead,
// but things would have been less obvious in the snippets.
//! [Include]

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variable_value;

static Elements::Logging logger = Elements::Logging::getLogger("EleFitsTutorial");

///////////////////
// DECLARATIONS //
/////////////////

//! [Tuto records]
struct TutoRecords {
  Fits::Record<std::string> string_record;
  Fits::Record<int> int_record;
  Fits::Record<float> float_record;
  Fits::Record<std::complex<double>> complex_record;
};
//! [Tuto records]

//! [Tuto rasters]
struct TutoRasters {
  Fits::VecRaster<std::int16_t, 2> int16_raster2d;
  Fits::VecRaster<std::int32_t, 3> int32_raster3d;
  Fits::VecRaster<std::int64_t, 4> int64_raster4d;
};
//! [Tuto rasters]

//! [Tuto columns]
struct TutoColumns {
  Fits::VecColumn<std::string> string_column;
  Fits::VecColumn<std::int32_t> int32_column;
  Fits::VecColumn<float> float_column;
};
//! [Tuto columns]

TutoRecords create_records();
TutoRasters create_rasters();
TutoColumns create_columns();

void write_file(const std::string& filename);
void read_file(const std::string& filename);
void write_records(const Fits::Header& h);
void read_records(const Fits::Header& h);
void read_raster(const Fits::ImageRaster& du);
void read_columns(const Fits::BintableColumns& du);

///////////////////
// DATA CLASSES //
/////////////////

TutoRecords create_records() {

  logger.info("  Creating records...");

  //! [Create records]

  /* Create a record with unit and comment */

  Fits::Record<std::string> string_record("STRING", "VALUE", "unit", "comment");

  /* Create a record with keyword and value only */

  Fits::Record<int> int_record("INT", 0);

  /* Create a record from an initialization list */

  Fits::Record<float> float_record {"FLOAT", 3.14F, "", "A piece of Pi"};
  // This is often used as a shortcut to create records as function parameters.

  /* Generate a random record */

  auto complex_record = Fits::Test::generate_random_record<std::complex<double>>("COMPLEX");

  //! [Create records]

  return {string_record, int_record, float_record, complex_record};
}

TutoRasters create_rasters() {

  logger.info("  Creating rasters...");

  //! [Create rasters]

  /* Initialize and later fill a raster */

  Fits::VecRaster<std::int16_t, 2> int16_raster2d({4, 3});
  for (const auto& position : int16_raster2d.domain()) {
    int16_raster2d[position] = position[0] + position[1];
  }
  // This demonstrates the iteration over positions;
  // It is possible to use two nested loops instead.

  /* Create a raster from a vector */

  std::vector<std::int32_t> int32_vec(16 * 9 * 3, 0);
  // ... do what you have to do with the vector, and then move it to the raster ...
  Fits::VecRaster<std::int32_t, 3> int32_raster3d({16, 9, 3}, std::move(int32_vec));
  // Instead of moving a vector, it's also possible to work with
  // a raw pointer with the PtrRaster class.

  /* Generate a random raster */

  auto int64_raster4d = Fits::Test::RandomRaster<std::int64_t, 4>({17, 9, 3, 24});

  //! [Create rasters]

  return {int16_raster2d, int32_raster3d, int64_raster4d};
}

TutoColumns create_columns() {

  logger.info("  Creating columns...");

  //! [Create columns]

  /* Initialize and later fill a column */

  Fits::VecColumn<std::string> string_column({"STRING", "unit", 3}, 100);
  // String columns must be wide-enough to hold each character.
  for (long i = 0; i < string_column.row_count(); ++i) {
    string_column(i) = std::to_string(i);
  }
  // operator() takes two parameters: the row index, and repeat index (=0 by default)

  /* Create a column from a vector */

  std::vector<std::int32_t> int32_vec(100);
  // ... do what you have to do with the vector, and then move it to the column ...
  Fits::VecColumn<std::int32_t> int32_column({"INT32", "", 1}, std::move(int32_vec));
  // Analogously to rasters, columns can be managed with the lightweight PtrColumn classe.

  /* Generate a random column */

  auto float_column = Fits::Test::RandomVectorColumn<float>(8, 100);

  //! [Create columns]

  return {string_column, int32_column, float_column};
}

//////////////
// WRITING //
////////////

void write_file(const std::string& filename) {

  logger.info("Creating a MEF file...");

  //! [Create a MEF file]

  Fits::MefFile f(filename, Fits::FileMode::Create);

  //! [Create a MEF file]

  const auto rasters = create_rasters();

  logger.info("  Writing image HDUs...");

  //! [Create image extensions]

  /* Fill the header and data units */

  const auto& image1 = f.append_image("IMAGE1", {}, rasters.int32_raster3d);

  /* Fill the header only (for now) */

  const auto& image2 = f.append_null_image<std::int16_t>("IMAGE2", {}, rasters.int16_raster2d.shape());

  //! [Create image extensions]

  //! [Write an image]

  image2.raster().write(rasters.int16_raster2d);

  //! [Write an image]

  const auto columns = create_columns();

  logger.info("  Writing binary table HDUs...");

  //! [Create binary table extensions]

  /* Fill the header and data units */

  const auto& table1 =
      f.append_bintable("TABLE1", {}, columns.string_column, columns.int32_column, columns.float_column);

  /* Fill the header unit only (for now) */

  const auto& table2 = f.append_bintable_header(
      "TABLE2",
      {},
      columns.string_column.info(),
      columns.int32_column.info(),
      columns.float_column.info());

  //! [Create binary table extensions]

  //! [Write columns]

  /* Write a single column */

  table2.columns().write(columns.string_column);

  /* Write several columns */

  table2.columns().write_seq(columns.int32_column, columns.float_column);

  //! [Write columns]

  /* Write records */

  write_records(f.primary().header());

  /* Mute "unused variable" warnings */

  (void)image1;
  (void)table1;

  // File is closed at destruction of f.
}

void write_records(const Fits::Header& h) {

  const auto records = create_records();

  logger.info("  Writing records...");

  //! [Write records]

  /* Write a single record */

  h.write(records.string_record);

  /* Write several records */

  h.write_seq(records.int_record, records.float_record, records.complex_record);

  /* Update using initialization lists */

  h.write_seq<Fits::RecordMode::UpdateExisting>(
      Fits::Record<int>("INT", 1),
      Fits::Record<float>("FLOAT", 3.14159F, "", "A larger piece of Pi"),
      Fits::Record<std::complex<double>>("COMPLEX", {180., 90.}));

  //! [Write records]
}

//////////////
// READING //
////////////

void read_file(const std::string& filename) {

  logger.info("Reading the MEF file...");

  //! [Open a MEF file]

  Fits::MefFile f(filename, Fits::FileMode::Read);

  //! [Open a MEF file]

  logger.info("  Accessing HDUs...");

  //! [Access HDUs]

  /* Access the Primary HDU */

  const auto& primary = f.primary();
  const auto primary_index = primary.index();
  // Indices are 0-based.

  /* Access an HDU by its index */

  const auto& image2 = f.access<Fits::ImageHdu>(2);
  const auto image_name = image2.read_name();

  /* Access an HDU by its name */

  const auto& table1 = f.find<Fits::BintableHdu>("TABLE1");
  const auto table_index = table1.index();
  // If several HDUs have the same name, the first one is returned.

  //! [Access HDUs]

  logger.info() << "    Primary index: " << primary_index;
  logger.info() << "    Name of the second extension: " << image_name;
  logger.info() << "    Index of the 'TABLE1' extension: " << table_index;

  read_records(primary.header());
  read_raster(image2.raster());
  read_columns(table1.columns());
}

void read_records(const Fits::Header& h) {

  logger.info("  Reading records...");

  //! [Read records]

  /* Read a single record */

  const auto int_record = h.parse<int>("INT");

  // Records can be sliced as their value for immediate use:
  const int intValue = h.parse<int>("INT");

  /* Read several records */

  const auto some_records = h.parse_seq(
      Fits::as<std::string>("STRING"),
      Fits::as<int>("INT"),
      Fits::as<float>("FLOAT"),
      Fits::as<std::complex<double>>("COMPLEX"));
  const auto& third_record = std::get<2>(some_records);

  /* Read as VariantValue */

  const auto variant_records = h.parse_seq<>({"INT", "COMPLEX"});
  const auto complex_record = variant_records.as<std::complex<double>>("COMPLEX");

  /* Read as a user-defined structure */

  const auto tutoRecords = h.parse_struct<TutoRecords>(
      Fits::as<std::string>("STRING"),
      Fits::as<int>("INT"),
      Fits::as<float>("FLOAT"),
      Fits::as<std::complex<double>>("COMPLEX"));
  const auto& string_record = tutoRecords.string_record;

  //! [Read records]

  logger.info() << "    " << int_record.keyword << " = " << int_record.value << " " << int_record.unit;
  logger.info() << "    INT value: " << intValue;
  logger.info() << "    " << third_record.keyword << " = " << third_record.value << " " << third_record.unit;
  logger.info() << "    " << complex_record.keyword << " = " << complex_record.value.real() << " + "
                << complex_record.value.imag() << "j " << complex_record.unit;
  logger.info() << "    " << string_record.keyword << " = " << string_record.value << " " << string_record.unit;
}

void read_raster(const Fits::ImageRaster& du) {

  logger.info("  Reading a raster...");

  //! [Read a raster]

  const auto image = du.read<std::int16_t, 2>();

  const auto& first_pixel = image[{0, 0}];
  const auto& last_pixel = image.at({-1, -1});
  // `operator[]` performs no bound checking, while `at` does and enables backward indexing.

  //! [Read a raster]

  logger.info() << "    First pixel: " << first_pixel;
  logger.info() << "    Last pixel: " << last_pixel;
}

void read_columns(const Fits::BintableColumns& du) {

  logger.info("  Reading columns...");

  //! [Read columns]

  /* Read a single column */

  const auto vector_column = du.read<double>("VECTOR");

  /* Read several columns by their name */

  const auto by_name = du.read_seq(Fits::as<std::string>("STRING"), Fits::as<std::int32_t>("INT32"));
  const auto& string_column = std::get<0>(by_name);

  /* Read several columns by their index */

  const auto by_index = du.read_seq(Fits::as<std::string>(0), Fits::as<std::int32_t>(1));
  const auto& int_column = std::get<1>(by_index);

  /* Use values */

  const auto& first_string = string_column(0);
  const auto& first_int = int_column(0);
  const auto& last_float = vector_column.at(-1, -1);
  // There is no operator[]() for columns, because vector columns require 2 indices (row and repeat).
  // operator()() performs no bound checking, while at() does and enables backward indexing.

  //! [Read columns]

  logger.info() << "    First string: " << first_string;
  logger.info() << "    First int: " << first_int;
  logger.info() << "    Last float: " << last_float;
}

//////////////
// PROGRAM //
////////////

class EleFitsTutorial : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    auto options = Fits::ProgramOptions::fromAuxFile("Tutorial.txt");
    options.positional("output", value<std::string>()->default_value("/tmp/tuto.fits"), "Output file");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    const auto filename = args["output"].as<std::string>();

    logger.info() << "---";
    logger.info() << "Hello, EleFits " << Fits::version() << "!";
    logger.info() << "---";

    write_file(filename);

    logger.info() << "---";

    read_file(filename);

    logger.info() << "---";
    logger.info() << "The end!";
    logger.info() << "---";

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsTutorial)
