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

void set_strategy(MefFile& out, const std::string& algo, char lossless)
{
  auto type = CompressionType::Lossless;
  if (lossless == 'i') {
    type = CompressionType::LosslessInts;
  } else if (lossless == 'n') {
    type = CompressionType::Lossy;
  } else if (lossless != 'y') {
    throw FitsError(std::string("Unknown losslessness: ") + lossless);
  }

  Quantization q0;
  Quantization q(Tile::rms);
  auto qints = (type == CompressionType::Lossy) ? q : q0;
  auto qfloats = (type == CompressionType::Lossless) ? q0 : q;

  Scaling s0;
  Scaling s(Tile::rms * 2.5);
  auto sints = (type == CompressionType::Lossy) ? s : s0;
  auto sfloats = (type == CompressionType::Lossless) ? s0 : s;

  if (algo == "AUTO") {
    out.strategy(CompressAuto(type));
  } else if (algo == "GZIP") {
    out.strategy(CompressFloats<Gzip>(Tile::adaptive(), qfloats), CompressInts<Gzip>(Tile::adaptive(), qints));
  } else if (algo == "SGZIP") {
    out.strategy(
        CompressFloats<ShuffledGzip>(Tile::adaptive(), qfloats),
        CompressInts<ShuffledGzip>(Tile::adaptive(), qints));
  } else if (algo == "RICE") {
    if (type != CompressionType::Lossless) {
      out.strategy(CompressFloats<Rice>(Tile::adaptive(), qfloats));
    }
    out.strategy(CompressInts<Rice>(Tile::adaptive(), qints));
  } else if (algo == "HCOMPRESS") {
    if (type != CompressionType::Lossless) {
      out.strategy(CompressFloats<HCompress>(Tile::adaptive(), qfloats, sfloats));
    }
    out.strategy(CompressInts<HCompress>(Tile::adaptive(), qints, sints));
  } else if (algo == "PLIO") {
    out.strategy(CompressInts<Plio>(Tile::adaptive(), q0));
  } else if (algo != "NONE") {
    throw FitsError(std::string("Unknown compression algorithm: ") + algo);
  }
}

class EleFitsCompress : public Elements::Program {
public:

  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override
  {
    auto options = ProgramOptions::from_aux_file("Compress.txt");
    options.positional("input", value<std::string>(), "Input file");
    options.positional("output", value<std::string>(), "Output file (if ends with .gz, compress externally)");
    options.named(
        "algo",
        value<std::string>()->default_value("AUTO"),
        "Compression algorithm (NONE, GZIP, SGZIP, RICE, HCOMPRESS, PLIO, AUTO)");
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
    const auto algo = args["algo"].as<std::string>();
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

    /* Enable compression (or not) */
    set_strategy(compressed, algo, lossless);

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
