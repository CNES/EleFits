// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestColumn.h"

namespace Euclid {
namespace Fits {
namespace Test {

#define GENERATE_COLUMN(type, name) getColumn<type>() = generateColumn<type>(#name, repeatCount, rowCount);

RandomTable::RandomTable(long repeatCount, long rowCount) : columns {} {
  ELEFITS_FOREACH_COLUMN_TYPE(GENERATE_COLUMN)
}

constexpr long RandomTable::columnCount;

SmallTable::SmallTable() :
    extname("MESSIER"), nums {45, 7, 31}, radecs {{56.8500F, 24.1167F}, {268.4667F, -34.7928F}, {10.6833F, 41.2692F}},
    names {"Pleiades", "Ptolemy Cluster", "Andromeda Galaxy"}, distsMags {0.44, 1.6, 0.8, 3.3, 2900.0, 3.4},
    numCol({"ID", "", 1}, nums.size(), nums.data()), radecCol({"RADEC", "deg", 1}, radecs.size(), radecs.data()),
    nameCol({"NAME", "", 68}, names.size(), names.data()), // TODO 68?
    distMagCol({"DIST_MAG", "kal", 2}, distsMags.size(), distsMags.data()) {}

} // namespace Test
} // namespace Fits
} // namespace Euclid
