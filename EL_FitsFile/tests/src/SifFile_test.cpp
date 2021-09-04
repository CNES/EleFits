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

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsFile/FitsFileFixture.h"
#include "EL_FitsFile/SifFile.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(SifFile_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(simple_image_test, Test::NewSifFile) {
  BOOST_TEST(boost::filesystem::is_regular_file(this->filename()));
  Test::SmallRaster input; // TODO RandomRaster
  const std::string keyword = "KEYWORD";
  const int value = 8;
  this->header().writeRecord(keyword, value);
  this->writeRaster(input);
  this->close();
  this->open(this->filename(), FileMode::Read); // Reopen as read-only
  const auto record = this->header().parseRecord<int>(keyword);
  BOOST_TEST((record == value));
  const auto output = this->readRaster<float>();
  BOOST_TEST(input.vector() == output.vector());
  BOOST_TEST(input.approx(output));
  remove(this->filename().c_str());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
