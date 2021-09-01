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
#include "EL_FitsFile/Hdu.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(Header_test, Test::TemporarySifFile)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(keyword_error_test) {
  const std::string keyword = "TEST";
  KeywordExistsError kee(keyword);
  KeywordNotFoundError knfe(keyword);
  BOOST_TEST(kee.keyword == keyword);
  BOOST_TEST(knfe.keyword == keyword);
}

BOOST_AUTO_TEST_CASE(syntax_test) {

  /* Setup */
  const auto& h = header().header();
  const Record<int> i("I", 1);
  const Record<float> f("F", 3.14);
  const auto t = std::make_tuple(i, f);
  const auto v = RecordVector<> { i, f };
  struct S {
    int i;
    float f;
  };

  /* Single write */
  h.write("I", 0);
  h.write(i);
  h.write<RecordMode::CreateNew>("I", 0);
  h.write<RecordMode::CreateNew>("I", 0);

  /* Heterogeneous write */
  h.writeSeq(i, f);
  h.writeSeq(t);
  h.writeSeqIn({ "I" }, i, f);
  h.writeSeqIn({ "F" }, t);
  h.writeSeq<RecordMode::CreateNew>(i, f);
  h.writeSeq<RecordMode::CreateNew>(t);

  /* Homogeneous write */
  h.writeSeq(v.vector);
  h.writeSeqIn({ "I" }, v.vector);
  h.writeSeq<RecordMode::CreateNew>(v.vector);

  /* Global read */
  h.readAll(~KeywordCategory::Comment);
  h.parseAll(~KeywordCategory::Comment);

  /* Single read */
  h.parse<int>(i.keyword);
  h.parseOr<int>(i.keyword, 0);
  h.parseOr(i);

  /* Heterogeneous read */
  h.parseSeq(Named<int>("I"), Named<float>("F"));
  h.parseSeqOr(std::make_tuple(Record<int>("I", 0), Record<float>("F", 3.14)));
  h.parseStruct<S>(Named<int>("I"), Named<float>("F"));

  /* Homogeneous read */
  h.parseSeq<VariantValue>({ "I", "F" });
}

BOOST_AUTO_TEST_CASE(checksum_test) {
  const auto& h = header().header();
  BOOST_CHECK_THROW(h.verifyChecksums(), ChecksumError);
  h.updateChecksums();
  BOOST_CHECK_NO_THROW(h.verifyChecksums());
  h.write("DATASUM", std::string(""));
  BOOST_CHECK_THROW(h.verifyChecksums(), ChecksumError);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
