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
  Fits::Record<std::string> stringRecord;
  Fits::Record<int> intRecord;
  Fits::Record<float> floatRecord;
  Fits::Record<std::complex<double>> complexRecord;
};
//! [Tuto records]

//! [Tuto rasters]
struct TutoRasters {
  Fits::VecRaster<std::int16_t, 2> int16Raster2D;
  Fits::VecRaster<std::int32_t, 3> int32Raster3D;
  Fits::VecRaster<std::int64_t, 4> int64Raster4D;
};
//! [Tuto rasters]

//! [Tuto columns]
struct TutoColumns {
  Fits::VecColumn<std::string> stringColumn;
  Fits::VecColumn<std::int32_t> int32Column;
  Fits::VecColumn<float> float32Column;
};
//! [Tuto columns]

TutoRecords createRecords();
TutoRasters createRasters();
TutoColumns createColumns();

void writeMefFile(const std::string& filename);
void readMefFile(const std::string& filename);
void writeRecords(const Fits::Header& h);
void readRecords(const Fits::Header& h);
void readRaster(const Fits::ImageRaster& du);
void readColumns(const Fits::BintableColumns& du);

///////////////////
// DATA CLASSES //
/////////////////

TutoRecords createRecords() {

  logger.info("  Creating records...");

  //! [Create records]

  /* Create a record with unit and comment */

  Fits::Record<std::string> stringRecord("STRING", "VALUE", "unit", "comment");

  /* Create a record with keyword and value only */

  Fits::Record<int> intRecord("INT", 0);

  /* Create a record from an initialization list */

  Fits::Record<float> floatRecord {"FLOAT", 3.14F, "", "A piece of Pi"};
  // This is often used as a shortcut to create records as function parameters.

  /* Generate a random record */

  auto complexRecord = Fits::Test::generateRandomRecord<std::complex<double>>("COMPLEX");

  //! [Create records]

  return {stringRecord, intRecord, floatRecord, complexRecord};
}

TutoRasters createRasters() {

  logger.info("  Creating rasters...");

  //! [Create rasters]

  /* Initialize and later fill a raster */

  Fits::VecRaster<std::int16_t, 2> int16Raster2D({4, 3});
  for (const auto& position : int16Raster2D.domain()) {
    int16Raster2D[position] = position[0] + position[1];
  }
  // This demonstrates the iteration over positions;
  // It is possible to use two nested loops instead.

  /* Create a raster from a vector */

  std::vector<std::int32_t> int32Vec(16 * 9 * 3, 0);
  // ... do what you have to do with the vector, and then move it to the raster ...
  Fits::VecRaster<std::int32_t, 3> int32Raster3D({16, 9, 3}, std::move(int32Vec));
  // Instead of moving a vector, it's also possible to work with
  // a raw pointer with the PtrRaster class.

  /* Generate a random raster */

  auto int64Raster4D = Fits::Test::RandomRaster<std::int64_t, 4>({17, 9, 3, 24});

  //! [Create rasters]

  return {int16Raster2D, int32Raster3D, int64Raster4D};
}

TutoColumns createColumns() {

  logger.info("  Creating columns...");

  //! [Create columns]

  /* Initialize and later fill a column */

  Fits::VecColumn<std::string> stringColumn({"STRING", "unit", 3}, 100);
  // String columns must be wide-enough to hold each character.
  for (long i = 0; i < stringColumn.rowCount(); ++i) {
    stringColumn(i) = std::to_string(i);
  }
  // operator() takes two parameters: the row index, and repeat index (=0 by default)

  /* Create a column from a vector */

  std::vector<std::int32_t> int32Vec(100);
  // ... do what you have to do with the vector, and then move it to the column ...
  Fits::VecColumn<std::int32_t> int32Column({"INT32", "", 1}, std::move(int32Vec));
  // Analogously to rasters, columns can be managed with the lightweight PtrColumn classe.

  /* Generate a random column */

  auto float32Column = Fits::Test::RandomVectorColumn<float>(8, 100);

  //! [Create columns]

  return {stringColumn, int32Column, float32Column};
}

//////////////
// WRITING //
////////////

void writeMefFile(const std::string& filename) {

  logger.info("Creating a MEF file...");

  //! [Create a MEF file]

  Fits::MefFile f(filename, Fits::FileMode::Create);

  //! [Create a MEF file]

  const auto rasters = createRasters();

  logger.info("  Writing image HDUs...");

  //! [Create image extensions]

  /* Fill the header and data units */

  const auto& image1 = f.assignImageExt("IMAGE1", rasters.int32Raster3D);

  /* Fill the header only (for now) */

  const auto& image2 = f.initImageExt<std::int16_t>("IMAGE2", rasters.int16Raster2D.shape());

  //! [Create image extensions]

  //! [Write an image]

  image2.raster().write(rasters.int16Raster2D);

  //! [Write an image]

  const auto columns = createColumns();

  logger.info("  Writing binary table HDUs...");

  //! [Create binary table extensions]

  /* Fill the header and data units */

  const auto& table1 = f.assignBintableExt("TABLE1", columns.stringColumn, columns.int32Column, columns.float32Column);

  /* Fill the header unit only (for now) */

  const auto& table2 = f.initBintableExt(
      "TABLE2",
      columns.stringColumn.info(),
      columns.int32Column.info(),
      columns.float32Column.info());

  //! [Create binary table extensions]

  //! [Write columns]

  /* Write a single column */

  table2.columns().write(columns.stringColumn);

  /* Write several columns */

  table2.columns().writeSeq(columns.int32Column, columns.float32Column);

  //! [Write columns]

  /* Write records */

  writeRecords(f.primary().header());

  /* Mute "unused variable" warnings */

  (void)image1;
  (void)table1;

  // File is closed at destruction of f.
}

void writeRecords(const Fits::Header& h) {

  const auto records = createRecords();

  logger.info("  Writing records...");

  //! [Write records]

  /* Write a single record */

  h.write(records.stringRecord);

  /* Write several records */

  h.writeSeq(records.intRecord, records.floatRecord, records.complexRecord);

  /* Update using initialization lists */

  h.writeSeq<Fits::RecordMode::UpdateExisting>(
      Fits::Record<int>("INT", 1),
      Fits::Record<float>("FLOAT", 3.14159F, "", "A larger piece of Pi"),
      Fits::Record<std::complex<double>>("COMPLEX", {180., 90.}));

  //! [Write records]
}

//////////////
// READING //
////////////

void readMefFile(const std::string& filename) {

  logger.info("Reading the MEF file...");

  //! [Open a MEF file]

  Fits::MefFile f(filename, Fits::FileMode::Read);

  //! [Open a MEF file]

  logger.info("  Accessing HDUs...");

  //! [Access HDUs]

  /* Access the Primary HDU */

  const auto& primary = f.primary();
  const auto primaryIndex = primary.index();
  // Indices are 0-based.

  /* Access an HDU by its index */

  const auto& image2 = f.access<Fits::ImageHdu>(2);
  const auto imageName = image2.readName();

  /* Access an HDU by its name */

  const auto& table1 = f.find<Fits::BintableHdu>("TABLE1");
  const auto tableIndex = table1.index();
  // If several HDUs have the same name, the first one is returned.

  //! [Access HDUs]

  logger.info() << "    Primary index: " << primaryIndex;
  logger.info() << "    Name of the second extension: " << imageName;
  logger.info() << "    Index of the 'TABLE1' extension: " << tableIndex;

  readRecords(primary.header());
  readRaster(image2.raster());
  readColumns(table1.columns());
}

void readRecords(const Fits::Header& h) {

  logger.info("  Reading records...");

  //! [Read records]

  /* Read a single record */

  const auto intRecord = h.parse<int>("INT");

  // Records can be sliced as their value for immediate use:
  const int intValue = h.parse<int>("INT");

  /* Read several records */

  const auto someRecords = h.parseSeq(
      Fits::as<std::string>("STRING"),
      Fits::as<int>("INT"),
      Fits::as<float>("FLOAT"),
      Fits::as<std::complex<double>>("COMPLEX"));
  const auto& thirdRecord = std::get<2>(someRecords);

  /* Read as VariantValue */

  const auto variantRecords = h.parseSeq<>({"INT", "COMPLEX"});
  const auto complexRecord = variantRecords.as<std::complex<double>>("COMPLEX");

  /* Read as a user-defined structure */

  const auto tutoRecords = h.parseStruct<TutoRecords>(
      Fits::as<std::string>("STRING"),
      Fits::as<int>("INT"),
      Fits::as<float>("FLOAT"),
      Fits::as<std::complex<double>>("COMPLEX"));
  const auto& stringRecord = tutoRecords.stringRecord;

  //! [Read records]

  logger.info() << "    " << intRecord.keyword << " = " << intRecord.value << " " << intRecord.unit;
  logger.info() << "    INT value: " << intValue;
  logger.info() << "    " << thirdRecord.keyword << " = " << thirdRecord.value << " " << thirdRecord.unit;
  logger.info() << "    " << complexRecord.keyword << " = " << complexRecord.value.real() << " + "
                << complexRecord.value.imag() << "j " << complexRecord.unit;
  logger.info() << "    " << stringRecord.keyword << " = " << stringRecord.value << " " << stringRecord.unit;
}

void readRaster(const Fits::ImageRaster& du) {

  logger.info("  Reading a raster...");

  //! [Read a raster]

  const auto image = du.read<std::int16_t, 2>();

  const auto& firstPixel = image[{0, 0}];
  const auto& lastPixel = image.at({-1, -1});
  // `operator[]` performs no bound checking, while `at` does and enables backward indexing.

  //! [Read a raster]

  logger.info() << "    First pixel: " << firstPixel;
  logger.info() << "    Last pixel: " << lastPixel;
}

void readColumns(const Fits::BintableColumns& du) {

  logger.info("  Reading columns...");

  //! [Read columns]

  /* Read a single column */

  const auto vectorColumn = du.read<double>("VECTOR");

  /* Read several columns by their name */

  const auto byName = du.readSeq(Fits::as<std::string>("STRING"), Fits::as<std::int32_t>("INT32"));
  const auto& stringColumn = std::get<0>(byName);

  /* Read several columns by their index */

  const auto byIndex = du.readSeq(Fits::as<std::string>(0), Fits::as<std::int32_t>(1));
  const auto& intColumn = std::get<1>(byIndex);

  /* Use values */

  const auto& firstString = stringColumn(0);
  const auto& firstInt = intColumn(0);
  const auto& lastFloat = vectorColumn.at(-1, -1);
  // There is no operator[]() for columns, because vector columns require 2 indices (row and repeat).
  // operator()() performs no bound checking, while at() does and enables backward indexing.

  //! [Read columns]

  logger.info() << "    First string: " << firstString;
  logger.info() << "    First int: " << firstInt;
  logger.info() << "    Last float: " << lastFloat;
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

    writeMefFile(filename);

    logger.info() << "---";

    readMefFile(filename);

    logger.info() << "---";
    logger.info() << "The end!";
    logger.info() << "---";

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsTutorial)
