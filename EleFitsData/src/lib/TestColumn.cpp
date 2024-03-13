// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestColumn.h"

namespace Euclid {
namespace Fits {
namespace Test {

#define GENERATE_COLUMN(type, name) get_column<type>() = generate_column<type>(#name, repeat_count, row_count);

RandomTable::RandomTable(Linx::Index repeat_count, Linx::Index row_count) : columns {}
{
  ELEFITS_FOREACH_COLUMN_TYPE(GENERATE_COLUMN)
}

#undef GENERATE_COLUMN

constexpr Linx::Index RandomTable::column_count;

SmallTable::SmallTable() :
    extname("MESSIER"), nums {45, 7, 31}, radecs {{56.8500F, 24.1167F}, {268.4667F, -34.7928F}, {10.6833F, 41.2692F}},
    names {"Pleiades", "Ptolemy Cluster", "Andromeda Galaxy"}, dists_mags {0.44, 1.6, 0.8, 3.3, 2900.0, 3.4},
    num_col({"ID", "", 1}, nums.size(), nums.data()), radec_col({"RADEC", "deg", 1}, radecs.size(), radecs.data()),
    name_col({"NAME", "", 68}, names.size(), names.data()), // TODO 68?
    dist_mag_col({"DIST_MAG", "kal", 2}, dists_mags.size() / 2, dists_mags.data())
{}

} // namespace Test
} // namespace Fits
} // namespace Euclid
