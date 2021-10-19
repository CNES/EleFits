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
void writeRecords(const Fits::Hdu& hdu);
void readRecords(const Fits::Hdu& hdu);
void readRaster(const Fits::ImageHdu& hdu);
void readColumns(const Fits::BintableHdu& hdu);

///////////////////
// DATA CLASSES //
/////////////////

TutoRecords createRecords() {

  //! [Create records]

  logger.info("  Creating records...");

  /* Create a record with unit and comment */

  Fits::Record<std::string> stringRecord("STRING", "VALUE", "unit", "comment");

  /* Create a record with keyword and value only */

  Fits::Record<int> intRecord("INT", 0);

  /* Create a record from an initialization list */

  Fits::Record<float> floatRecord { "FLOAT", 3.14F, "", "A piece of Pi" };
  // This is often used as a shortcut to create records as function parameters.

  /* Generate a random record */

  auto complexRecord = Fits::Test::generateRandomRecord<std::complex<double>>("COMPLEX");

  //! [Create records]

  return { stringRecord, intRecord, floatRecord, complexRecord };
}

TutoRasters createRasters() {

  //! [Create rasters]

  logger.info("  Creating rasters...");

  /* Initialize and later fill a raster */

  Fits::VecRaster<std::int16_t, 2> int16Raster2D({ 4, 3 });
  for (const auto& position : int16Raster2D.domain()) {
    int16Raster2D[position] = position[0] + position[1];
  }
  // This demonstrates the iteration over positions;
  // It is possible to use two nested loops instead.

  /* Create a raster from a vector */

  std::vector<std::int32_t> int32Vec(16 * 9 * 3, 0);
  // ... do what you have to do with the vector, and then move it to the raster ...
  Fits::VecRaster<std::int32_t, 3> int32Raster3D({ 16, 9, 3 }, std::move(int32Vec));
  // Instead of moving a vector, it's also possible to work with
  // a raw pointer with the PtrRaster class.

  /* Generate a random raster */

  auto int64Raster4D = Fits::Test::RandomRaster<std::int64_t, 4>({ 17, 9, 3, 24 });

  //! [Create rasters]

  return { int16Raster2D, int32Raster3D, int64Raster4D };
}

TutoColumns createColumns() {

  //! [Create columns]

  logger.info("  Creating columns...");

  /* Initialize and later fill a column */

  Fits::VecColumn<std::string> stringColumn({ "STRING", "unit", 3 }, 100);
  // String columns must be wide-enough to hold each character.
  for (long i = 0; i < stringColumn.rowCount(); ++i) {
    stringColumn(i) = std::to_string(i);
  }
  // operator() takes two parameters: the row index, and repeat index (=0 by default)

  /* Create a column from a vector */

  std::vector<std::int32_t> int32Vec(100);
  // ... do what you have to do with the vector, and then move it to the column ...
  Fits::VecColumn<std::int32_t> int32Column({ "INT32", "", 1 }, std::move(int32Vec));
  // Analogously to rasters, columns can be managed with the lightweight PtrColumn classe.

  /* Generate a random column */

  auto float32Column = Fits::Test::RandomVectorColumn<float>(8, 100);

  //! [Create columns]

  return { stringColumn, int32Column, float32Column };
}

//////////////
// WRITING //
////////////

void writeMefFile(const std::string& filename) {

  //! [Create a MEF file]

  logger.info("Creating a MEF file...");

  Fits::MefFile f(filename, Fits::FileMode::Create);

  //! [Create a MEF file]

  const auto rasters = createRasters();

  //! [Create image extensions]

  logger.info("  Writing image HDUs...");

  /* Initialize HDU first and write raster later */

  const auto& image1 = f.initImageExt<std::int16_t, 2>("IMAGE1", rasters.int16Raster2D.shape());
  // ... do something with the extension ...
  image1.raster().write(rasters.int16Raster2D);

  /* Assign at creation */

  const auto& image2 = f.assignImageExt("IMAGE2", rasters.int32Raster3D);

  //! [Create image extensions]

  const auto columns = createColumns();

  //! [Create binary table extensions]

  logger.info("  Writing binary table HDUs...");

  /* Initialize HDU first and write columns later */

  const auto& table1 = f.initBintableExt<std::string, int, float>(
      "TABLE1",
      columns.stringColumn.info(),
      columns.int32Column.info(),
      columns.float32Column.info());
  table1.columns().writeSeq(columns.stringColumn, columns.int32Column, columns.float32Column);

  /* Assign at creation */

  const auto& table2 = f.assignBintableExt("TABLE2", columns.stringColumn, columns.int32Column, columns.float32Column);

  //! [Create binary table extensions]

  /* Write records */

  writeRecords(f.accessPrimary<>());

  /* Mute "unused variable" warnings */

  (void)image2;
  (void)table2;

  // File is closed at destruction of f.
}

void writeRecords(const Fits::Hdu& hdu) {

  const auto records = createRecords();

  //! [Write records]

  logger.info("  Writing records...");

  /* Write a single record */

  hdu.header().write(records.stringRecord);

  /* Write several records */

  hdu.header().writeSeq(records.intRecord, records.floatRecord, records.complexRecord);

  /* Update using initialization lists */

  hdu.header().writeSeq(
      Fits::Record<int>("INT", 1),
      Fits::Record<float>("FLOAT", 3.14159F, "", "A larger piece of Pi"),
      Fits::Record<std::complex<double>>("COMPLEX", { 180., 90. }));

  //! [Write records]
}

//////////////
// READING //
////////////

void readMefFile(const std::string& filename) {

  //! [Open a MEF file]

  logger.info("Reading the MEF file...");

  Fits::MefFile f(filename, Fits::FileMode::Read);

  //! [Open a MEF file]

  //! [Access HDUs]

  logger.info("  Accessing HDUs...");

  /* Access the Primary HDU */

  const auto& primary = f.primary();
  logger.info() << "    Primary index: " << primary.index();
  // Indices are 0-based.

  /* Access an HDU by its index */

  const auto& image2 = f.access<Fits::ImageHdu>(2);
  logger.info() << "    Name of the second extension: " << image2.readName();

  /* Access an HDU by its name */

  const auto& table1 = f.accessFirst<Fits::BintableHdu>("TABLE1");
  // If several HDUs have the same name, the first one is returned.
  logger.info() << "    Index of the 'TABLE1' extension: " << table1.index();

  //! [Access HDUs]

  readRecords(primary);
  readRaster(image2);
  readColumns(table1);
}

void readRecords(const Fits::Hdu& hdu) {

  //! [Read records]

  logger.info("  Reading records...");

  /* Read a single record */

  auto intRecord = hdu.header().parse<int>("INT");
  logger.info() << "    " << intRecord.keyword << " = " << intRecord.value << " " << intRecord.unit;

  // Records can be sliced as their value for immediate use:
  int intValue = hdu.header().parse<int>("INT");
  logger.info() << "    INT value: " << intValue;

  /* Read several records */

  auto someRecords = hdu.header().parseSeq(
      Fits::Named<std::string>("STRING"),
      Fits::Named<int>("INT"),
      Fits::Named<float>("FLOAT"),
      Fits::Named<std::complex<double>>("COMPLEX"));
  auto thirdRecord = std::get<2>(someRecords);
  logger.info() << "    " << thirdRecord.keyword << " = " << thirdRecord.value << " " << thirdRecord.unit;

  /* Read as VariantValue */

  auto variantRecords = hdu.header().parseSeq<>({ "INT", "COMPLEX" });
  auto complexRecord = variantRecords.as<std::complex<double>>("COMPLEX");
  logger.info() << "    " << complexRecord.keyword << " = " << complexRecord.value.real() << " + "
                << complexRecord.value.imag() << "j " << complexRecord.unit;

  /* Read as a user-defined structure */

  auto tutoRecords = hdu.header().parseStruct<TutoRecords>(
      Fits::Named<std::string>("STRING"),
      Fits::Named<int>("INT"),
      Fits::Named<float>("FLOAT"),
      Fits::Named<std::complex<double>>("COMPLEX"));
  auto stringRecord = tutoRecords.stringRecord;
  logger.info() << "    " << stringRecord.keyword << " = " << stringRecord.value << " " << stringRecord.unit;

  //! [Read records]
}

void readRaster(const Fits::ImageHdu& hdu) {

  //! [Read a raster]

  logger.info("  Reading a raster...");

  const auto image = hdu.readRaster<std::int32_t, 3>();

  const auto& firstPixel = image[{ 0, 0, 0 }];
  const auto& lastPixel = image.at({ -1, -1, -1 });
  // `operator[]` performs no bound checking, while `at` does and enables backward indexing.

  logger.info() << "    First pixel: " << firstPixel;
  logger.info() << "    Last pixel: " << lastPixel;

  //! [Read a raster]
}

void readColumns(const Fits::BintableHdu& hdu) {

  //! [Read columns]

  logger.info("  Reading columns...");

  /* Read a single column */

  const auto vectorColumn = hdu.readColumn<double>("VECTOR");

  /* Read several columns by their name */

  const auto byName = hdu.columns().readSeq(Fits::Named<std::string>("STRING"), Fits::Named<std::int32_t>("INT32"));
  const auto& stringColumn = std::get<0>(byName);

  /* Read several columns by their index */

  const auto byIndex = hdu.columns().readSeq(Fits::Indexed<std::string>(0), Fits::Indexed<std::int32_t>(1));
  const auto& intColumn = std::get<1>(byIndex);

  /* Use values */

  logger.info() << "    First string: " << stringColumn(0);
  logger.info() << "    First int: " << intColumn(0);
  logger.info() << "    Last float: " << vectorColumn.at(-1, -1);
  // There is no `operator[]` for columns, because vector columns require 2 indices (row and repeat).
  // `operator()` performs no bound checking, while `at` does and enables backward indexing.

  //! [Read columns]
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

    const std::string filename = args["output"].as<std::string>();

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
