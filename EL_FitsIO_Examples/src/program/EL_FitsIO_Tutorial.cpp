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

#include "EL_FitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <map>
#include <string>

//! [Include fixtures]
#include "EL_FitsData/TestColumn.h"
#include "EL_FitsData/TestRaster.h"
#include "EL_FitsData/TestRecord.h"
//! [Include fixtures]

//! [Include]
#include "EL_FitsFile/MefFile.h"

using namespace Euclid;
// EL_FitsIO classes are in the Euclid::FitsIO namespace.
// We could have be using namespace Euclid::FitsIO instead,
// but things would have been less obvious in the snippets.
//! [Include]

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variable_value;

static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Tutorial");

///////////////////
// DECLARATIONS //
/////////////////

//! [Tuto records]
struct TutoRecords {
  FitsIO::Record<std::string> stringRecord;
  FitsIO::Record<int> intRecord;
  FitsIO::Record<float> floatRecord;
  FitsIO::Record<std::complex<double>> complexRecord;
};
//! [Tuto records]

//! [Tuto rasters]
struct TutoRasters {
  FitsIO::VecRaster<std::int16_t, 2> int16Raster2D;
  FitsIO::VecRaster<std::int32_t, 3> int32Raster3D;
  FitsIO::VecRaster<std::int64_t, 4> int64Raster4D;
};
//! [Tuto rasters]

//! [Tuto columns]
struct TutoColumns {
  FitsIO::VecColumn<std::string> stringColumn;
  FitsIO::VecColumn<std::int32_t> int32Column;
  FitsIO::VecColumn<float> float32Column;
};
//! [Tuto columns]

TutoRecords createRecords();
TutoRasters createRasters();
TutoColumns createColumns();

void writeMefFile(const std::string& filename);
void readMefFile(const std::string& filename);
void writeRecords(const FitsIO::RecordHdu& hdu);
void readRecords(const FitsIO::RecordHdu& hdu);
void readRaster(const FitsIO::ImageHdu& hdu);
void readColumns(const FitsIO::BintableHdu& hdu);

///////////////////
// DATA CLASSES //
/////////////////

TutoRecords createRecords() {

  //! [Create records]

  logger.info("  Creating records...");

  /* Create a record with unit and comment */

  FitsIO::Record<std::string> stringRecord("STRING", "VALUE", "unit", "comment");

  /* Create a record with keyword and value only */

  FitsIO::Record<int> intRecord("INT", 0);

  /* Create a record from an initialization list */

  FitsIO::Record<float> floatRecord { "FLOAT", 3.14F, "", "A piece of Pi" };
  // This is often used as a shortcut to create records as function parameters.

  /* Generate a random record */

  auto complexRecord = FitsIO::Test::generateRandomRecord<std::complex<double>>("COMPLEX");

  //! [Create records]

  return { stringRecord, intRecord, floatRecord, complexRecord };
}

TutoRasters createRasters() {

  //! [Create rasters]

  logger.info("  Creating rasters...");

  /* Initialize and later fill a raster */

  FitsIO::VecRaster<std::int16_t, 2> int16Raster2D({ 4, 3 });
  for (long y = 0; y < int16Raster2D.length<1>(); ++y) {
    for (long x = 0; x < int16Raster2D.length<0>(); ++x) {
      int16Raster2D[{ x, y }] = x + y;
    }
  }

  /* Create a raster from a vector */

  std::vector<std::int32_t> int32Vec(16 * 9 * 3);
  // ... do what you have to do with the vector, and then move it to the raster ...
  FitsIO::VecRaster<std::int32_t, 3> int32Raster3D({ 16, 9, 3 }, std::move(int32Vec));
  // Instead moving a vector, it's also possible to work with:
  // - a vector reference with the VecRefRaster class, or
  // - a raw pointer with the PtrRaster class.

  /* Generate a random raster */

  auto int64Raster4D = FitsIO::Test::RandomRaster<std::int64_t, 4>({ 17, 9, 3, 24 });

  //! [Create rasters]

  return { int16Raster2D, int32Raster3D, int64Raster4D };
}

TutoColumns createColumns() {

  //! [Create columns]

  logger.info("  Creating columns...");

  /* Initialize and later fill a column */

  FitsIO::VecColumn<std::string> stringColumn({ "STRING", "unit", 3 }, 100);
  // String columns must be wide-enough to hold each character.
  for (long i = 0; i < stringColumn.rowCount(); ++i) {
    stringColumn.vector()[i] = std::to_string(i);
  }

  /* Create a column from a vector */

  std::vector<std::int32_t> int32Vec(100);
  // ... do what you have to do with the vector, and then move it to the column ...
  FitsIO::VecColumn<std::int32_t> int32Column({ "INT32", "", 1 }, std::move(int32Vec));
  // Analogously to rasters, columns can be managed with the VecRefColumn and PtrColumn classes.

  /* Generate a random column */

  auto float32Column = FitsIO::Test::RandomVectorColumn<float>(8, 100);

  //! [Create columns]

  return { stringColumn, int32Column, float32Column };
}

//////////////
// WRITING //
////////////

void writeMefFile(const std::string& filename) {

  //! [Create a MEF file]

  logger.info("Creating a MEF file...");

  FitsIO::MefFile f(filename, FitsIO::MefFile::Permission::Create);

  //! [Create a MEF file]

  const auto rasters = createRasters();

  //! [Create image extensions]

  logger.info("  Writing image HDUs...");

  /* Initialize HDU first and write raster later */

  const auto& image1 = f.initImageExt<std::int16_t, 2>("IMAGE1", rasters.int16Raster2D.shape);
  // ... do something with the extension ...
  image1.writeRaster(rasters.int16Raster2D);

  /* Assign at creation */

  const auto& image2 = f.assignImageExt("IMAGE2", rasters.int32Raster3D);

  //! [Create image extensions]

  const auto columns = createColumns();

  //! [Create binary table extensions]

  logger.info("  Writing binary table HDUs...");

  /* Initialize HDU first and write columns later */

  const auto& table1 = f.initBintableExt<std::string, int, float>(
      "TABLE1",
      columns.stringColumn.info,
      columns.int32Column.info,
      columns.float32Column.info);
  table1.writeColumns(columns.stringColumn, columns.int32Column, columns.float32Column);

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

void writeRecords(const FitsIO::RecordHdu& hdu) {

  const auto records = createRecords();

  //! [Write records]

  logger.info("  Writing records...");

  /* Write a single record */

  hdu.writeRecord(records.stringRecord);

  /* Write several records */

  hdu.writeRecords(records.intRecord, records.floatRecord, records.complexRecord);

  /* Update using initialization lists */

  hdu.updateRecords<int, float, std::complex<double>>(
      { "INT", 1 },
      { "FLOAT", 3.14159F, "", "A larger piece of Pi" },
      { "COMPLEX", { 180., 90. } });
  // With inititialization lists, template parameters must be explicit.
  // Each "write" method has an "update" counterpart with the same signature.

  //! [Write records]
}

//////////////
// READING //
////////////

void readMefFile(const std::string& filename) {

  //! [Open a MEF file]

  logger.info("Reading the MEF file...");

  FitsIO::MefFile f(filename, FitsIO::MefFile::Permission::Read);

  //! [Open a MEF file]

  //! [Access HDUs]

  logger.info("  Accessing HDUs...");

  /* Access the Primary HDU */

  const auto& primary = f.accessPrimary<FitsIO::RecordHdu>();
  // Our primary contains only metadata, which is why we request a RecordHdu.
  logger.info() << "    Primary index: " << primary.index();
  // Indices are 0-based in the FitsIO namespace.

  /* Access an HDU by its index */

  const auto& image2 = f.access<FitsIO::ImageHdu>(2);
  logger.info() << "    Name of the second extension: " << image2.readName();

  /* Access an HDU by its name */

  const auto& table1 = f.accessFirst<FitsIO::BintableHdu>("TABLE1");
  // If several HDUs have the same name, the first one is returned.
  logger.info() << "    Index of the 'TABLE1' extension: " << table1.index();

  //! [Access HDUs]

  readRecords(primary);
  readRaster(image2);
  readColumns(table1);
}

void readRecords(const FitsIO::RecordHdu& hdu) {

  //! [Read records]

  logger.info("  Reading records...");

  /* Read a single record */

  auto intRecord = hdu.parseRecord<int>("INT");
  logger.info() << "    " << intRecord.keyword << " = " << intRecord.value << " " << intRecord.unit;

  // Records can be sliced as their value for immediate use:
  int intValue = hdu.parseRecord<int>("INT");
  logger.info() << "    INT value: " << intValue;

  /* Read several records */

  auto someRecords = hdu.parseRecords(
      FitsIO::Named<std::string>("STRING"),
      FitsIO::Named<int>("INT"),
      FitsIO::Named<float>("FLOAT"),
      FitsIO::Named<std::complex<double>>("COMPLEX"));
  auto thirdRecord = std::get<2>(someRecords);
  logger.info() << "    " << thirdRecord.keyword << " = " << thirdRecord.value << " " << thirdRecord.unit;

  /* Read as boost::any */

  auto anyRecords = hdu.parseRecordVector<boost::any>({ "INT", "COMPLEX" });
  auto complexRecord = anyRecords.as<std::complex<double>>("COMPLEX");
  logger.info() << "    " << complexRecord.keyword << " = " << complexRecord.value.real() << " + "
                << complexRecord.value.imag() << "j " << complexRecord.unit;

  /* Read as a user-defined structure */

  auto tutoRecords = hdu.parseRecordsAs<TutoRecords>(
      FitsIO::Named<std::string>("STRING"),
      FitsIO::Named<int>("INT"),
      FitsIO::Named<float>("FLOAT"),
      FitsIO::Named<std::complex<double>>("COMPLEX"));
  auto stringRecord = tutoRecords.stringRecord;
  logger.info() << "    " << stringRecord.keyword << " = " << stringRecord.value << " " << stringRecord.unit;

  //! [Read records]
}

void readRaster(const FitsIO::ImageHdu& hdu) {

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

void readColumns(const FitsIO::BintableHdu& hdu) {

  //! [Read columns]

  logger.info("  Reading columns...");

  /* Read a single column */

  const auto vectorColumn = hdu.readColumn<double>("VECTOR");

  /* Read several columns by their name */

  const auto byName = hdu.readColumns(FitsIO::Named<std::string>("STRING"), FitsIO::Named<std::int32_t>("INT32"));
  const auto& stringColumn = std::get<0>(byName);

  /* Read several columns by their index */

  const auto byIndex = hdu.readColumns(FitsIO::Indexed<std::string>(0), FitsIO::Indexed<std::int32_t>(1));
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

class EL_FitsIO_Tutorial : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    auto options = FitsIO::ProgramOptions::fromAuxFile("Tutorial.txt");
    options.positional("output", value<std::string>()->default_value("/tmp/tuto.fits"), "Output file");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {

    const std::string filename = args["output"].as<std::string>();

    logger.info() << "---";
    logger.info() << "Hello, EL_FitsIO " << FitsIO::version() << "!";
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

MAIN_FOR(EL_FitsIO_Tutorial)
