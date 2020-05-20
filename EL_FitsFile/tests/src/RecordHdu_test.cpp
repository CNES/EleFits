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

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"

#include "EL_FitsFile/MefFile.h"
#include "EL_FitsFile/SifFile.h"

#include "EL_FitsFile/RecordHdu.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RecordHdu_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( continued_str_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  SifFile f(filename, SifFile::Permission::TEMPORARY);
  const auto& h = f.header();
  const std::string short_str = "S";
  const std::string long_str =
      "This is probably one of the longest strings "
      "that I have ever written in a serious code.";
  BOOST_CHECK_GT(long_str.length(), FLEN_VALUE);
  h.write_record<std::string>("SHORT", short_str);
  BOOST_CHECK_THROW(h.parse_record<std::string>("LONGSTRN"), std::exception);
  h.write_record<std::string>("LONG", long_str);
  const auto output = h.parse_record<std::string>("LONG");
  h.parse_record<std::string>("LONGSTRN");
  BOOST_CHECK_EQUAL(output.value, long_str);
}

BOOST_AUTO_TEST_CASE( rename_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  MefFile f(filename, MefFile::Permission::TEMPORARY);
  const auto& h = f.init_record_ext("A");
  BOOST_CHECK_EQUAL(h.index(), 2);
  BOOST_CHECK_EQUAL(h.name(), "A");
  h.rename("B");
  BOOST_CHECK_EQUAL(h.name(), "B");
  h.delete_record("EXTNAME");
  BOOST_CHECK_EQUAL(h.name(), "");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
