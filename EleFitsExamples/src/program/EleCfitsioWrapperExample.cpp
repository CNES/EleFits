// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/CfitsioWrapper.h"
#include "EleFitsData/TestColumn.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;
using namespace Cfitsio;

class EleCfitsioWrapperExample : public Elements::Program {

public:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options;
    options.positional("output", value<std::string>()->default_value("/tmp/test.fits"), "Output file");
    return options.asPair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleCfitsioWrapperExample");

    const auto filename = args["output"].as<std::string>();

    logger.info();

    logger.info() << "Creating FITS file: " << filename;
    //! [Create FITS]
    auto fptr = FileAccess::createAndOpen(filename, FileAccess::CreatePolicy::OverWrite);
    //! [Create FITS]
    logger.info() << "Writing new record: VALUE = 1";
    //! [Write record]
    HeaderIo::writeRecord<int>(fptr, {"VALUE", 1});
    //! [Write record]
    logger.info() << "Updating record: VALUE = 2";
    //! [Update record]
    HeaderIo::updateRecord<int>(fptr, {"VALUE", 2});
    //! [Update record]

    logger.info();

    logger.info() << "Creating binary table extension: SMALLTBL";
    Fits::Test::SmallTable table; // Predefined table for testing purpose
    //! [Create binary table ext]
    HduAccess::assignBintableExtension(fptr, "SMALLTBL", table.numCol, table.radecCol, table.nameCol, table.distMagCol);
    //! [Create binary table ext]

    logger.info();

    logger.info() << "Creating image extension: SMALLIMG";
    Fits::Test::SmallRaster raster; // Predefined image raster for testing purpose
    //! [Create image ext]
    HduAccess::assignImageExtension(fptr, "SMALLIMG", raster);
    //! [Create image ext]
    logger.info() << "Writing record: STRING = string";
    Fits::Record<std::string> strRecord("STRING", "string");
    logger.info() << "Writing record: INTEGER = 8";
    Fits::Record<int> intRecord("INTEGER", 8);
    HeaderIo::writeRecords(fptr, strRecord, intRecord);

    logger.info();

    logger.info() << "Closing file.";
    //! [Close FITS]
    FileAccess::close(fptr);
    //! [Close FITS]

    logger.info();

    logger.info() << "Reopening file.";
    //! [Open FITS]
    fptr = FileAccess::open(filename, FileAccess::OpenPolicy::ReadOnly);
    //! [Open FITS]
    //! [Read record]
    const auto recordValue = HeaderIo::parseRecord<int>(fptr, "VALUE");
    //! [Read record]
    logger.info() << "Reading record: VALUE = " << recordValue;

    logger.info();

    logger.info() << "Reading binary table.";
    //! [Find HDU by name]
    HduAccess::gotoName(fptr, "SMALLTBL");
    //! [Find HDU by name]
    //! [Get HDU index]
    const auto index = HduAccess::currentIndex(fptr);
    //! [Get HDU index]
    logger.info() << "HDU index: " << index;
    //! [Read column]
    const auto ids = BintableIo::readColumn<int>(fptr, "ID").vector();
    const auto firstCell = ids[0];
    //! [Read column]
    logger.info() << "First id: " << firstCell;
    const auto names = BintableIo::readColumn<std::string>(fptr, "NAME").vector();
    logger.info() << "Last name: " << names[names.size() - 1];

    logger.info();

    logger.info() << "Reading image.";
    //! [Find HDU by index]
    HduAccess::gotoIndex(fptr, 3);
    //! [Find HDU by index]
    //! [Get HDU name]
    const auto extname = HduAccess::currentName(fptr);
    //! [Get HDU name]
    logger.info() << "Name of HDU #3: " << extname;
    const auto records = HeaderIo::parseRecords<std::string, int>(fptr, {"STRING", "INTEGER"});
    logger.info() << "Reading record: STRING = " << std::get<0>(records).value;
    logger.info() << "Reading record: INTEGER = " << std::get<1>(records).value;
    HduAccess::gotoName(fptr, "SMALLIMG");
    //! [Read raster]
    const auto image = ImageIo::readRaster<float>(fptr);
    const auto firstPixel = image[{0, 0}];
    const auto lastPixel = image.at({-1, -1}); // at() allows backward indexing
    //! [Read raster]
    logger.info() << "First pixel: " << firstPixel;
    logger.info() << "Last pixel: " << lastPixel;

    logger.info();

    logger.info() << "Reclosing file.";
    FileAccess::close(fptr);

    logger.info();

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleCfitsioWrapperExample)
