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

#include "EL_FitsFile/FitsFileFixture.h"
#include "EL_FitsFile/HduIterator.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduIterator_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(range_loop_over_all_hdus, Test::TemporaryMefFile) {
  this->initRecordExt("1");
  this->initRecordExt("2");
  int count = 0;
  for (const auto& hdu : *this) {
    BOOST_TEST(hdu.matches(Image));
    count++;
  }
  BOOST_TEST(count == this->hduCount());
}

BOOST_FIXTURE_TEST_CASE(range_loop_over_selected_hdus, Test::TemporaryMefFile) {
  ColumnInfo<float> info { "COL", "", 1 };
  std::vector<std::string> names { "", "BINTABLE1", "BINTABLE2", "IMAGE" };
  this->initBintableExt(names[1], info);
  this->initBintableExt(names[2], info);
  this->initImageExt<float, 2>(names[3], { 1, 1 });

  int count = 0;
  std::vector<std::string> readNames;

  for (const auto& hdu : this->select<ImageHdu>(HduCategory::Primary)) {
    const std::string name = hdu.readName();
    BOOST_TEST(name == names[0]);
    readNames.push_back(name);
    BOOST_TEST(hdu.matches(HduCategory::Image));
    count++;
  }
  BOOST_TEST(count == 1);

  for (const auto& hdu : this->select<BintableHdu>(HduCategory::Bintable)) {
    readNames.push_back(hdu.readName());
    BOOST_TEST(hdu.matches(HduCategory::Bintable & HduCategory::Ext));
    count++;
  }
  BOOST_TEST(count == 3);

  for (const auto& hdu : this->select<ImageHdu>(HduCategory::ImageExt)) {
    readNames.push_back(hdu.readName());
    BOOST_TEST(hdu.matches(HduCategory::Image - HduCategory::Primary));
    count++;
  }
  BOOST_TEST(count == 4);

  BOOST_TEST(readNames == names);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
