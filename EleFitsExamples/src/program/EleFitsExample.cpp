// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleFits/MefFile.h"
#include "EleFitsData/TestColumn.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;
using namespace Fits;

class EleFitsExample : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options;
    options.positional("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsExample");

    const std::string filename = args["output"].as<std::string>();

    logger.info();
    {
      logger.info() << "Creating FITS file: " << filename;
      //! [Create FITS]
      MefFile f(filename, FileMode::Overwrite);
      //! [Create FITS]
      const auto& primary = f.primary();
      logger.info() << "Writing new record: VALUE = 1";
      //! [Write record]
      primary.header().write("VALUE", 1);
      //! [Write record]
      logger.info() << "Updating record: VALUE = 2";
      //! [Update record]
      primary.header().write("VALUE", 2);
      //! [Update record]

      logger.info();

      Test::SmallTable table; // Predefined table for testing purpose
      logger.info() << "Creating binary table extension: SMALLTBL";
      //! [Create binary table ext]
      f.appendBintable("SMALLTBL", {}, table.num_col, table.radec_col, table.name_col, table.dist_mag_col);
      //! [Create binary table ext]

      logger.info();

      Test::SmallRaster raster; // Predefined image raster for testing purpose
      logger.info() << "Creating image extension: SMALLIMG";
      //! [Create image ext]
      const auto& ext = f.appendImage("SMALLIMG", {}, raster);
      //! [Create image ext]
      logger.info() << "Writing record: STRING = string";
      Record<std::string> string_record("STRING", "string");
      logger.info() << "Writing record: INTEGER = 8";
      Record<int> integer_record("INTEGER", 8);
      ext.header().writeSeq(string_record, integer_record);

      logger.info();

      logger.info() << "Closing file.";
      //! [Close FITS]
      f.close(); // We close the file manually for demo purpose, but this is done by the destructor otherwise
      //! [Close FITS]
    }
    logger.info();
    {

      logger.info() << "Reopening file.";
      //! [Open FITS]
      MefFile f(filename, FileMode::Read);
      //! [Open FITS]
      //! [Read record]
      const auto record_value = f.primary().header().parse<int>("VALUE");
      //! [Read record]
      logger.info() << "Reading record: VALUE = " << record_value;

      logger.info();

      logger.info() << "Reading binary table.";
      //! [Find HDU by name]
      const auto& bintable = f.find<BintableHdu>("SMALLTBL");
      //! [Find HDU by name]
      //! [Get HDU index]
      const auto index = bintable.index();
      //! [Get HDU index]
      logger.info() << "HDU index: " << index;
      //! [Read column]
      const auto ids = bintable.columns().read<int>("ID").vector();
      const auto first_entry = ids[0];
      //! [Read column]
      logger.info() << "First id: " << first_entry;
      const auto names = bintable.columns().read<std::string>("NAME").vector();
      logger.info() << "Last name: " << names[names.size() - 1];

      logger.info();

      logger.info() << "Reading image.";
      //! [Find HDU by index]
      const auto& ext2 = f[2];
      //! [Find HDU by index]
      //! [Get HDU name]
      const auto extname = ext2.readName();
      //! [Get HDU name]
      logger.info() << "Name of HDU #3: " << extname;
      const auto records = ext2.header().parseSeq(as<std::string>("STRING"), as<int>("INTEGER"));
      logger.info() << "Reading record: STRING = " << std::get<0>(records).value;
      logger.info() << "Reading record: INTEGER = " << std::get<1>(records).value;
      const auto& image = f.find<ImageHdu>("SMALLIMG");
      //! [Read raster]
      const auto raster = image.readRaster<float>();
      const auto first_pixel = raster[{0, 0}];
      const auto last_pixel = raster.at({-1, -1}); // at() allows backward indexing
      //! [Read raster]
      logger.info() << "First pixel: " << first_pixel;
      logger.info() << "Last pixel: " << last_pixel;

      logger.info();

      logger.info() << "File will be closed at execution end.";

      logger.info();

      return Elements::ExitCode::OK;
    } // File is closed by destructor
  }
};

MAIN_FOR(EleFitsExample)
