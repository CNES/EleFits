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

#include "EleCfitsioWrapper/CfitsioFixture.h"

#include "EleCfitsioWrapper/FileWrapper.h"

namespace Euclid {
namespace Fits {
namespace Test {

MinimalFile::MinimalFile() : tmp(), filename(tmp.path().string()), fptr(nullptr) {
  fptr = Cfitsio::FileAccess::createAndOpen(filename, Cfitsio::FileAccess::CreatePolicy::OverWrite);
}

MinimalFile::~MinimalFile() {
  Cfitsio::FileAccess::close(fptr);
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
