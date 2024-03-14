// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "Linx/Run/ProgramOptions.h"

#include <map>
#include <string>

using namespace Euclid::Fits;

/*
 * Some properties of an `AstroObj`.
 */
struct AstroObjInfo {
  Linx::Index comb_size;
  Linx::Index dith_count;
  Linx::Index dith1d_size;
  Linx::Position<2> dith2d_shape;
};

/*
 * Write records in the Primary.
 */
void write_primary_header(const Header& h, Linx::Index nobj)
{
  Record<std::int64_t> nobj_record {"N_OBJ", nobj, "", "number of objects in the package"};
  Record<std::string> telescope_record {"TELESCOP", "EUCLID", "", "telescope name"};
  Record<std::string> instrument_record {"INSTRUME", "NISP", "", "instrument name"};
  h.write_n(nobj_record, telescope_record, instrument_record);
}

/*
 * Write a binary table extension which represents one of several `AstroObj`s.
 */
const BintableHdu& write_ext(MefFile& f, const std::string& name, const AstroObjInfo& info, Linx::Index row_count)
{
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

  return f.append_bintable(
      name,
      {},
      make_column(std::move(objectid_info), std::move(objectid_data)),
      make_column(std::move(radec_info), std::move(radec_data)),
      make_column(std::move(exptime_info), std::move(exptime_data)),
      make_column(std::move(comb_signal_info), std::move(com_signal_data)),
      make_column(std::move(comb_var_info), std::move(comb_var_data)),
      make_column(std::move(ptgid_info), std::move(ptgid_data)),
      make_column(std::move(dith1d_signal_info), std::move(dith1d_signal_data)),
      make_column(std::move(dith1d_var_info), std::move(dith1d_var_data)),
      make_column(std::move(dith2d_signal_info), std::move(dith2d_signal_data)),
      make_column(std::move(dith2d_var_info), std::move(dith2d_var_data)));
}

/**
 * Insert quality columns to a binary table HDU.
 */
void insert_columns(const BintableColumns& du, const AstroObjInfo& info)
{
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

  const auto row_count = du.read_row_count();
  auto comb_qual_data = Test::generate_random_vector<std::int32_t>(comb_size * row_count);
  auto dith1d_qual_data = Test::generate_random_vector<std::int32_t>(dith1d_size * dith_count * row_count);
  auto dith2d_qual_data =
      Test::generate_random_vector<std::int32_t>(dith2d_width * dith2d_height * dith_count * row_count);

  /* Insert before variance columns */

  du.insert_null(du.read_index("COMBINED1D_VAR"), comb_qual_info);
  du.insert_null(du.read_index("DITH1D_VAR"), dith1d_qual_info);
  du.insert_null(du.read_index("DITH2D_VAR"), dith2d_qual_info);
  du.write_n(
      make_column(std::move(comb_qual_info), std::move(comb_qual_data)),
      make_column(std::move(dith1d_qual_info), std::move(dith1d_qual_data)),
      make_column(std::move(dith2d_qual_info), std::move(dith2d_qual_data)));
}

int main(int argc, char const* argv[])
{
  Linx::ProgramOptions options("Generate a random AstroObj file, as specified in the SpectrumLib.");
  options.positional<std::string>("output", "Output file", "/tmp/astroobj.fits");
  options.named<Linx::Index>("nobj", "AstroObj count per HDU", 1);
  options.named<Linx::Index>("nhdu", "HDU count", 1);
  options.named<Linx::Index>("nbin", "Wavelength bin count", 1000);
  options.named<Linx::Index>("ndith", "Dither count per AstroObj", 4);
  options.named<Linx::Index>("height", "Dither 2D height", 15);
  options.flag("qual", "Flag to write quality columns");
  options.parse(argc, argv);

  Elements::Logging logger = Elements::Logging::getLogger("EleFitsGenerateAstroObj");

  std::string filename = options.as<std::string>("output");
  const auto nobj = options.as<Linx::Index>("nobj");
  const auto nhdu = options.as<Linx::Index>("nhdu");
  const auto nbin = options.as<Linx::Index>("nbin");
  const auto ndith = options.as<Linx::Index>("ndith");
  const auto height = options.as<Linx::Index>("height");
  const auto qual = options.as<bool>("qual");

  AstroObjInfo info {nbin, ndith, nbin, {nbin, height}};

  logger.info() << "Creating FITS file: " << filename;
  MefFile f(filename, FileMode::Overwrite);

  logger.info() << "Writing metadata";
  write_primary_header(f.primary().header(), nobj * nhdu);

  for (Linx::Index i = 0; i < nhdu; ++i) {
    logger.info() << "Writing HDU " << i + 1;
    const auto& ext = write_ext(f, std::to_string(i + 1), info, nobj);
    if (qual) {
      insert_columns(ext.columns(), info);
    }
  }

  logger.info() << "Done.";
  return 0;
}
