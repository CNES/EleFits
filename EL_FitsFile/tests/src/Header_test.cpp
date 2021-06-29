/**
 * @file tests/src/Header_test.cpp
 * @date 06/27/21
 * @author user
 *
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
#include "EL_FitsFile/RecordHdu.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(Header_test, Test::TemporaryMefFile)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(syntax_test) {
  Header h(accessPrimary<>());
  const Record<int> i("I", 1);
  const Record<float> f("F", 3.14);
  const auto t = std::make_tuple(i, f);

  h.readAll(~KeywordCategory::Comment);
  h.parseAll(~KeywordCategory::Comment);

  /* Single parse */
  h.parse1<int>(i.keyword);
  h.parse1Or<int>(i.keyword, 0);
  h.parse1Or(i);

  /* Multi parse */
  h.parseN(Named<int>("I"), Named<float>("F"));
  h.parseN(std::make_tuple(Named<int>("I"), Named<float>("F")));
  h.parseNOr(std::make_tuple(Record<int>("I", 0), Record<float>("F", 3.14)));

  /* Single write */
  h.write1("I", 0);
  h.write1(i);
  h.write1<Header::WriteMode::CreateNew>("I", 0);
  h.write1<Header::WriteMode::CreateNew>("I", 0);

  /* Heterogeneous write */
  h.writeN(i, f);
  h.writeN(t);
  h.writeN<Header::WriteMode::CreateNew>(i, f);
  h.writeN<Header::WriteMode::CreateNew>(t);

  /* Homogeneous write */
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
