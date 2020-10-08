/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "EL_FitsData/TestColumn.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

#define GENERATE_COLUMN(type, name) getColumn<type>() = generateColumn<type>(#name, repeatCount, rowCount);

RandomTable::RandomTable(long repeatCount, long rowCount) : columns {} {
  EL_FITSIO_FOREACH_COLUMN_TYPE(GENERATE_COLUMN)
}

constexpr long RandomTable::columnCount;

SmallTable::SmallTable() :
    extname("MESSIER"),
    nums { 45, 7, 31 },
    radecs { { 56.8500F, 24.1167F }, { 268.4667F, -34.7928F }, { 10.6833F, 41.2692F } },
    names { "Pleiades", "Ptolemy Cluster", "Andromeda Galaxy" },
    distsMags { 0.44, 1.6, 0.8, 3.3, 2900.0, 3.4 },
    numCol({ "ID", "", 1 }, nums),
    radecCol({ "RADEC", "deg", 1 }, radecs),
    nameCol({ "NAME", "", 68 }, names), // TODO 68?
    distMagCol({ "DIST_MAG", "kal", 2 }, distsMags) {
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid
