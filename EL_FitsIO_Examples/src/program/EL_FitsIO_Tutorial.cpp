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

#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Temporary.h"

#include "EL_FitsData/TestUtils.h"

//! [Include]
#include "EL_FitsFile/MefFile.h"

using Euclid::FitsIO::Record;
using Euclid::FitsIO::VecColumn;
using Euclid::FitsIO::VecRaster;
using Euclid::FitsIO::MefFile;
using Euclid::FitsIO::BintableHdu;
using Euclid::FitsIO::ImageHdu;
using Euclid::FitsIO::RecordHdu;
//! [Include]

using boost::program_options::options_description;
using boost::program_options::variable_value;
using boost::program_options::value;

static Elements::Logging logger = Elements::Logging::getLogger("EL_FitsIO_Tutorial");

VecRaster<float, 3> createRaster() {
  //! [Create and fill a raster]
  long width = 16, height = 9, depth = 3;
  VecRaster<float, 3> raster({ width, height, depth });
  for (long z = 0; z < depth; ++z) {
    for (long y = 0; y < height; ++y) {
      for (long x = 0; x < width; ++x) {
        raster[{ x, y, z }] = float(x + y + z);
      }
    }
  }
  //! [Create and fill a raster]
  return raster;
}

struct TutoTable {
  VecColumn<std::string> nameCol;
  VecColumn<double> speed_col;
};

TutoTable createColumns() {
  //! [Create and fill a column]
  std::vector<std::string> nameData { "snail", "Antoine", "light", "Millennium Falcon" };
  std::vector<double> speedData { 1.3e-2, 1.4, 3.0e8, 4.5e8 };
  TutoTable table { VecColumn<std::string>({ "NAME", "", 42 }, std::move(nameData)),
                    VecColumn<double>({ "SPEED", "m/s", 1 }, std::move(speedData)) };
  //! [Create and fill a column]
  return table;
}

void readRecords(const RecordHdu &hdu) {

  logger.info() << "Reading some records:";

  //! [Read a single record]
  auto single = hdu.parseRecord<int>("INT");
  logger.info() << single.keyword << " = " << single.value << " " << single.unit;
  //! [Read a single record]

  //! [Read heterogeneous records]
  auto heterogeneous = hdu.parseRecords<std::string, float, std::complex<double>>({ "STRING", "FLOAT", "D_COMPLEX" });
  auto second = std::get<1>(heterogeneous);
  logger.info() << second.keyword << " = " << second.value << " " << second.unit;
  //! [Read heterogeneous records]

  //! [Read homogeneous records]
  auto homogeneous = hdu.parseRecordVector<int>({ "INT1", "INT2" });
  auto homogeneous1 = homogeneous["INT1"];
  logger.info() << homogeneous1.keyword << " = " << homogeneous1.value << " " << homogeneous1.unit;
  //! [Read homogeneous records]

  //! [Read std::any records]
  auto any = hdu.parseRecordVector<boost::any>({ "STRING", "FLOAT", "D_COMPLEX" });
  auto third = any.as<std::complex<double>>("D_COMPLEX"); // Cast boost::any value to std::complex<double>
  logger.info() << third.keyword << " = " << third.value.real() << "+" << third.value.imag() << "j";
  //! [Read std::any records]

  //! [Read a record struct]
  struct MyHeader {
    std::string someString;
    Record<float> someFloat;
    std::complex<double> someComplex;
  };

  auto header =
      hdu.parseRecordsAs<MyHeader, std::string, float, std::complex<double>>({ "STRING", "FLOAT", "D_COMPLEX" });
  logger.info() << header.someString;
  //! [Read a record struct]
}

void writeRecords(const RecordHdu &hdu) {
  // TODO
}

const RecordHdu &createRecordHdu(MefFile &file) {
  // TODO
  return file.accessPrimary<>();
}

void readRaster(const ImageHdu &hdu) {
  // TODO
}

void writeRaster(const ImageHdu &hdu) {
  // TODO
}

const ImageHdu &createImageHdu(MefFile &file) {
  // TODO
  return file.accessPrimary<ImageHdu>();
}

void readColumns(const BintableHdu &hdu) {
  // TODO single scalar, single vector, several mixed
}

void writeColumns(const BintableHdu &hdu) {
  // TODO idem
}

const BintableHdu &createBintableHdu(MefFile &file) {
  // TODO
  return file.accessPrimary<BintableHdu>();
}

class EL_FitsIO_Tutorial : public Elements::Program {

public:
  options_description defineSpecificProgramOptions() override {
    options_description options {};
    auto add = options.add_options();
    const auto defaultOutputFile = m_tempDir.path() / "test.fits";
    add("output", value<std::string>()->default_value(defaultOutputFile.string()), "Output file");
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, variable_value> &args) override {

    const std::string filename = args["output"].as<std::string>();

    logger.info() << "---";
    logger.info() << "Hello, EL_FitsIO " << Euclid::FitsIO::version() << "!";
    logger.info() << "---";

    //! [Open a MefFile]
    logger.info() << "Opening file: " << filename;
    MefFile f(filename, MefFile::Permission::Overwrite);
    //! [Open a MefFile]

    //! [Access primary HDU]
    const auto &primary = f.accessPrimary<>();
    //! [Access primary HDU]

    //! [Write and update a record]
    logger.info() << "Writing records to primary";
    primary.writeRecord("VALUE", 1);
    primary.updateRecord("VALUE", 2);
    //! [Write and update a record]

    //! [Create a complete record]
    Record<float> completeRecord("SPEED", 2.5, "m/s", "Already fast!");
    //! [Create a complete record]
    primary.writeRecord(completeRecord);

    const auto columns = createColumns();
    //! [Assign a bintable extension]
    logger.info() << "Assigning new Bintable HDU";
    f.assignBintableExt("TABLE", columns.nameCol, columns.speed_col);
    //! [Assign a bintable extension]

    const auto raster = createRaster();
    const auto shape = raster.shape;
    //! [Initialize an image extension]
    logger.info() << "Assigning new Image HDU";
    const auto &ext = f.initImageExt<float, 3>("Image", shape);
    ext.writeRaster(raster);
    //! [Initialize an image extension]

    //! [Write several records]
    logger.info() << "Writing several records at once";

    // Option 1: With concrete Record instances
    const Record<std::string> strRecord("STRING", "string");
    const Record<int> intRecord("INTEGER", 8);
    ext.writeRecords<std::string, int>(strRecord, intRecord);

    // Option 2: With temporary Record instances
    ext.writeRecords<std::string, int>({ "STR", "string" }, { "INT", 8 });
    //! [Write several records]

    logger.info() << "Here's the list of keywords in the extension:";
    const auto keywords = ext.readKeywords();
    for (const auto &k : keywords) {
      logger.info() << "    " << k;
    }

    logger.info() << "---";

    logger.info() << "Closing and reopening file in read-only mode";
    f.close();
    f.open(filename, MefFile::Permission::Read);

    logger.info() << "---";

    //! [Read a record]
    logger.info() << "Reading record in primary";
    const auto record = f.accessPrimary<>().parseRecord<int>("VALUE");
    logger.info() << "    VALUE = " << record.value;
    //! [Read a record]

    //! [Access an HDU by name]
    logger.info() << "Accessing bintable HDU by name";
    const auto &bintableExt = f.accessFirst<BintableHdu>("TABLE");
    logger.info() << "    Index: " << bintableExt.index();
    //! [Access an HDU by name]

    //! [Read bintable values]
    logger.info() << "Reading columns";
    const auto names = bintableExt.readColumn<std::string>("NAME").vector();
    const auto speeds = bintableExt.readColumn<double>("SPEED").vector();
    const auto slowestGuy = names[0];
    const auto maxSpeed = speeds[speeds.size() - 1];
    logger.info() << "    Slowest guy: " << slowestGuy;
    logger.info() << "    Max speed: " << maxSpeed;
    //! [Read bintable values]

    //! [Access an HDU by index]
    logger.info() << "Accessing image HDU by index";
    const auto &imageExt = f.access<ImageHdu>(3);
    logger.info() << "    Name: " << imageExt.readName();
    //! [Access an HDU by index]

    //! [Read several records]
    // Option 1. As a tuple
    auto records = imageExt.parseRecords<std::string, int>({ "STRING", "INTEGER" });
    const auto strValue = std::get<0>(records).value;
    const auto intValue = std::get<1>(records).value;
    logger.info() << "    String value from tuple: " << strValue;
    logger.info() << "    Integer value from tuple: " << intValue;

    // Option 2. As a user-defined struct
    struct Header {
      std::string strValue;
      int intValue;
    };

    auto header = imageExt.parseRecordsAs<Header, std::string, int>({ "STRING", "INTEGER" });
    logger.info() << "    String value from struct: " << header.strValue;
    logger.info() << "    Integer value from struct: " << header.intValue;
    //! [Read several records]

    //! [Read image values]
    const auto image = imageExt.readRaster<float, 3>();
    const auto &firstPixel = image[{ 0, 0 }];
    const auto width = image.length<0>();
    const auto height = image.length<1>();
    const auto depth = image.length<2>();
    const auto &lastPixel = image[{ width - 1, height - 1, depth - 1 }];
    logger.info() << "    First pixel: " << firstPixel;
    logger.info() << "    Last pixel: " << lastPixel;
    //! [Read image values]

    logger.info() << "---";
    logger.info() << "The end!";
    logger.info() << "---";

    return Elements::ExitCode::OK;
  }

private:
  Elements::TempDir m_tempDir;
};

MAIN_FOR(EL_FitsIO_Tutorial)
