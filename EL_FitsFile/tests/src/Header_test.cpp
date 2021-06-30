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
#include "EL_FitsFile/RecordHdu.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(Header_test, Test::TemporaryMefFile)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(keyword_error_test) {
  const std::string keyword = "TEST";
  KeywordExistsError kee(keyword);
  KeywordNotFoundError knfe(keyword);
  BOOST_TEST(kee.keyword == keyword);
  BOOST_TEST(knfe.keyword == keyword);
}

BOOST_AUTO_TEST_CASE(syntax_test) {
  const auto& h = accessPrimary<>().header();
  const Record<int> i("I", 1);
  const Record<float> f("F", 3.14);
  const auto t = std::make_tuple(i, f);
  const auto v = RecordVector<> { i, f };

  /* Single write */
  h.write1("I", 0);
  h.write1(i);
  h.write1<RecordMode::CreateNew>("I", 0);
  h.write1<RecordMode::CreateNew>("I", 0);

  /* Heterogeneous write */
  h.writeN(i, f);
  h.writeN(t);
  h.writeN<RecordMode::CreateNew>(i, f);
  h.writeN<RecordMode::CreateNew>(t);

  /* Homogeneous write */
  h.writeN(v.vector);
  h.writeN<RecordMode::CreateNew>(v.vector);

  /* Global read */
  h.readAll(~KeywordCategory::Comment);
  h.parseAll(~KeywordCategory::Comment);

  /* Single read */
  h.parse1<int>(i.keyword);
  h.parse1Or<int>(i.keyword, 0);
  h.parse1Or(i);

  /* Heterogeneous read */
  h.parseN(Named<int>("I"), Named<float>("F"));
  h.parseN(std::make_tuple(Named<int>("I"), Named<float>("F")));
  h.parseNOr(std::make_tuple(Record<int>("I", 0), Record<float>("F", 3.14)));

  /* Homogeneous read */
  h.parseN<VariantValue>({ "I", "F" });
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
