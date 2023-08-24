// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <iomanip> // setw, setfill
#include <ostream>
#include <sstream>
#include <string>

using boost::program_options::value;
using namespace Euclid::Fits;

class EleFitsCompress : public Elements::Program {
public:

  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override
  {
    auto options = ProgramOptions::from_aux_file("Compress.txt");
    options.positional("input", value<std::string>(), "Input file");
    options.positional("output", value<std::string>(), "Output file (if ends with .gz, compress externally)");
    options.flag("decompress", "Decompress instead of compressing"); // FIXME --algo none would be more flexible
    options.named("lossless", value<char>()->default_value('y'), "Losslessness: yes (y), no (n), integers only (i)");
    options.flag("primary", "Compress the Primary (as the first extension)");
    return options.as_pair();
  }

  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override
  {
    Elements::Logging logger = Elements::Logging::getLogger("EleFitsCompress");

    /* Read options */
    const auto input = args["input"].as<std::string>();
    const auto output = args["output"].as<std::string>();
    const auto decompress = args["decompress"].as<bool>();
    const auto lossless = args["lossless"].as<char>();
    const auto compress_primary = args["primary"].as<bool>();

    /* Open files */
    MefFile raw(input, FileMode::Read);
    const auto hdu_count = raw.hdu_count();
    logger.info() << "HDU count: " << hdu_count;

    MefFile compressed(output, FileMode::Create);

    /* Copy raw Primary */
    if (not compress_primary) {
      compressed.primary() = raw.primary();
    }

    /* Enable compression */
    if (not decompress) {
      if (lossless == 'y') {
        compressed.strategy(CompressAuto());
      } else if (lossless == 'i') {
        compressed.strategy(CompressAuto(CompressionType::LosslessInts));
      } else if (lossless == 'n') {
        compressed.strategy(CompressAuto(CompressionType::Lossy));
      } else {
        throw FitsError("Unknown compression type");
      }
    }

    /* Loop over HDUs or extensions */
    for (long i = 1 - compress_primary; i < hdu_count; ++i) {
      const auto& hdu = raw[i];
      logger.info() << "  HDU #" << i << ": " << hdu.read_name();
      compressed.append(hdu);
    }

    raw.close();
    compressed.close();
    logger.info("Done.");

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsCompress)
