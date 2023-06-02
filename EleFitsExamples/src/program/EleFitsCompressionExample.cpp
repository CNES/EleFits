// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleFits/MefFile.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

//#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid;

#define CREATE_IMAGE_FROM_TYPES(type, name) addGenericImageFromType<type>(f, #name, rows, cols);

static Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateImages");

template <typename T>
void addGenericImageFromType(Fits::MefFile& f, std::string typeName, long rows, long cols) {

  logger.info("- Adding Image with type " + typeName);

  // Create and add Raster with type T as extension
  Fits::VecRaster<T, 2> raster2D({cols, rows});
  std::iota(raster2D.begin(), raster2D.end(), 0);
  f.assignImageExt(typeName + "_IMAGE", raster2D);
}

/*
 * The program.
 */
class EleFitsCompressionExample : public Elements::Program {

public:
  // program options:
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Fits::ProgramOptions options("Write mef with all supported image extensions.");
    options.positional("output", value<std::string>()->default_value("/tmp/compression.fits"), "Output file");
    options.named("rows", value<long>()->default_value(20), "Number of rows");
    options.named("cols", value<long>()->default_value(30), "Number of columns");
    return options.asPair();
  }

  ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    const auto filename = args["output"].as<std::string>();
    const auto rows = args["rows"].as<long>();
    const auto cols = args["cols"].as<long>();

    logger.info("# Creating FITS file");

    // Create mef file to write the extensions in
    Fits::MefFile f(filename, Fits::FileMode::Overwrite);

    f.compress(Fits::Compression::None()); // TODO

    // Adds an image for each Raster type to the mef file
    ELEFITS_FOREACH_RASTER_TYPE(CREATE_IMAGE_FROM_TYPES)

    logger.info("# File created");

    return ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompressionExample)