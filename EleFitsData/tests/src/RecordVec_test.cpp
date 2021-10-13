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

#include "EleFitsData/RecordVec.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(RecordVec_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(records_are_found_by_their_keyword_test) {
  RecordVec<int> records(3);
  for (std::size_t i = 0; i < records.vector.size(); ++i) {
    records.vector[i].assign(std::to_string(i), int(i));
  }
  BOOST_TEST(records["1"].value == 1);
  BOOST_TEST(records["2"].value == 2);
  BOOST_CHECK_THROW(records["OOPS!"], std::exception);
}

BOOST_AUTO_TEST_CASE(records_are_cast_while_found_by_their_keyword_test) {
  RecordVec<double> records(1);
  records.vector[0].assign("PI", 3.14);
  auto pi_record = records.as<int>("PI");
  BOOST_TEST(pi_record.value == 3);
  int pi = records.as<int>("PI");
  BOOST_TEST(pi == 3);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
