/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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
