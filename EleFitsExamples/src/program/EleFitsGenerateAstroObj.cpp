// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "EleFitsUtils/ProgramOptions.h"
#include "ElementsKernel/ProgramHeaders.h"

#include <boost/program_options.hpp>
#include <map>
#include <string>

using boost::program_options::value;

using namespace Euclid::Fits;

/*
 * Some properties of an `AstroObj`.
 */
struct AstroObjInfo {
  long comb_size;
  long dith_count;
  long dith1d_size;
  Position<2> dith2d_shape;
};

/*
 * Write records in the Primary.
 */
void write_primary_header(const Header& h, long nobj) {
  Record<std::int64_t> nobj_record {"N_OBJ", nobj, "", "number of objects in the package"};
  Record<std::string> telescope_record {"TELESCOP", "EUCLID", "", "telescope name"};
  Record<std::string> instrument_record {"INSTRUME", "NISP", "", "instrument name"};
  h.writeSeq(nobj_record, telescope_record, instrument_record);
}

/*
 * Write a binary table extension which represents one of several `AstroObj`s.
 */
const BintableHdu& write_ext(MefFile& f, const std::string& name, const AstroObjInfo& info, long row_count) {

  /* Metadata */

  ColumnInfo<std::int64_t> objectid_info("OBJECT_ID");
  ColumnInfo<std::complex<float>> radec_info("RA_DEC", "deg");
  ColumnInfo<float> exptime_info("EXPTIME", "s");

  /* Combined infos */

  const auto comb_size = info.comb_size;

  ColumnInfo<float> comb_signal_info("COMBINED1D_SIGNAL", "erg", comb_size);
  ColumnInfo<float> comb_var_info("COMBINED1D_VAR", "erg^2", comb_size);

  /* Dithers infos */

  const auto dith_count = info.dith_count;
  const auto dith1d_size = info.dith1d_size;
  const auto dith2d_width = info.dith2d_shape[0];
  const auto dith2d_height = info.dith2d_shape[1];

  ColumnInfo<std::int64_t> ptgid_info("PTGID", "", dith_count);

  ColumnInfo<float, 2> dith1d_signal_info("DITH1D_SIGNAL", "erg", {dith1d_size, dith_count});
  ColumnInfo<float, 2> dith1d_var_info("DITH1D_VAR", "erg^2", {dith1d_size, dith_count});

  ColumnInfo<float, 3> dith2d_signal_info("DITH2D_SIGNAL", "erg", {dith2d_width, dith2d_height, dith_count});
  ColumnInfo<float, 3> dith2d_var_info("DITH2D_VAR", "erg^2", {dith2d_width, dith2d_height, dith_count});

  /* Random data */

  auto objectid_data = Test::generate_random_vector<std::int64_t>(row_count);
  auto radec_data = Test::generate_random_vector<std::complex<float>>(row_count);
  auto exptime_data = Test::generate_random_vector<float>(row_count);

  auto com_signal_data = Test::generate_random_vector<float>(comb_size * row_count);
  auto comb_var_data = Test::generate_random_vector<float>(comb_size * row_count);

  auto ptgid_data = Test::generate_random_vector<std::int64_t>(dith_count * row_count);

  auto dith1d_signal_data = Test::generate_random_vector<float>(dith1d_size * dith_count * row_count);
  auto dith1d_var_data = Test::generate_random_vector<float>(dith1d_size * dith_count * row_count);

  auto dith2d_signal_data = Test::generate_random_vector<float>(dith2d_width * dith2d_height * dith_count * row_count);
  auto dith2d_var_data = Test::generate_random_vector<float>(dith2d_width * dith2d_height * dith_count * row_count);

  /* Create and assign extension */

  return f.appendBintable(
      name,
      {},
      makeColumn(std::move(objectid_info), std::move(objectid_data)),
      makeColumn(std::move(radec_info), std::move(radec_data)),
      makeColumn(std::move(exptime_info), std::move(exptime_data)),
      makeColumn(std::move(comb_signal_info), std::move(com_signal_data)),
      makeColumn(std::move(comb_var_info), std::move(comb_var_data)),
      makeColumn(std::move(ptgid_info), std::move(ptgid_data)),
      makeColumn(std::move(dith1d_signal_info), std::move(dith1d_signal_data)),
      makeColumn(std::move(dith1d_var_info), std::move(dith1d_var_data)),
      makeColumn(std::move(dith2d_signal_info), std::move(dith2d_signal_data)),
      makeColumn(std::move(dith2d_var_info), std::move(dith2d_var_data)));
}

/**
 * Insert quality columns to a binary table HDU.
 */
void insert_columns(const BintableColumns& du, const AstroObjInfo& info) {

  /* Infos */

  const auto comb_size = info.comb_size;
  const auto dith_count = info.dith_count;
  const auto dith1d_size = info.dith1d_size;
  const auto dith2d_width = info.dith2d_shape[0];
  const auto dith2d_height = info.dith2d_shape[1];

  ColumnInfo<std::int32_t> comb_qual_info("COMBINED1D_QUALITY", "", comb_size);
  ColumnInfo<std::int32_t, 2> dith1d_qual_info("DITH1D_QUALITY", "", {dith1d_size, dith_count});
  ColumnInfo<std::int32_t, 3> dith2d_qual_info("DITH2D_QUALITY", "", {dith2d_width, dith2d_height, dith_count});

  /* Random data */

  const auto row_count = du.readRowCount();
  auto comb_qual_data = Test::generate_random_vector<std::int32_t>(comb_size * row_count);
  auto dith1d_qual_data = Test::generate_random_vector<std::int32_t>(dith1d_size * dith_count * row_count);
  auto dith2d_qual_data =
      Test::generate_random_vector<std::int32_t>(dith2d_width * dith2d_height * dith_count * row_count);

  /* Insert before variance columns */

  du.init(comb_qual_info, du.readIndex("COMBINED1D_VAR"));
  du.init(dith1d_qual_info, du.readIndex("DITH1D_VAR"));
  du.init(dith2d_qual_info, du.readIndex("DITH2D_VAR"));
  du.writeSeq(
      makeColumn(std::move(comb_qual_info), std::move(comb_qual_data)),
      makeColumn(std::move(dith1d_qual_info), std::move(dith1d_qual_data)),
      makeColumn(std::move(dith2d_qual_info), std::move(dith2d_qual_data)));
}

/*
 * The program.
 */
class EleFitsGenerateAstroObj : public Elements::Program {

public:
  /*
   * Program options.
   */
  std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
    Euclid::Fits::ProgramOptions options("Generate a random AstroObj file, as specified in the SpectrumLib.");
    options.positional("output", value<std::string>()->default_value("/tmp/astroobj.fits"), "Output file");
    options.named("nobj", value<long>()->default_value(1), "AstroObj count per HDU");
    options.named("nhdu", value<long>()->default_value(1), "HDU count");
    options.named("nbin", value<long>()->default_value(1000), "Wavelength bin count");
    options.named("ndith", value<long>()->default_value(4), "Dither count per AstroObj");
    options.named("height", value<long>()->default_value(15), "Dither 2D height");
    options.flag("qual", "Flag to write quality columns");
    return options.asPair();
  }

  /*
   * Run!
   */
  Elements::ExitCode mainMethod(std::map<std::string, VariableValue>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateAstroObj");

    std::string filename = args["output"].as<std::string>();
    const auto nobj = args["nobj"].as<long>();
    const auto nhdu = args["nhdu"].as<long>();
    const auto nbin = args["nbin"].as<long>();
    const auto ndith = args["ndith"].as<long>();
    const auto height = args["height"].as<long>();
    const auto qual = args["qual"].as<bool>();

    AstroObjInfo info {nbin, ndith, nbin, {nbin, height}};

    logger.info() << "Creating FITS file: " << filename;
    MefFile f(filename, FileMode::Overwrite);

    logger.info() << "Writing metadata";
    write_primary_header(f.primary().header(), nobj * nhdu);

    for (long i = 0; i < nhdu; ++i) {
      logger.info() << "Writing HDU " << i + 1;
      const auto& ext = write_ext(f, std::to_string(i + 1), info, nobj);
      if (qual) {
        insert_columns(ext.columns(), info);
      }
    }

    logger.info() << "Done.";
    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(EleFitsGenerateAstroObj)
