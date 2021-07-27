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

#include "EL_FitsIO_Validation/CfitsioBenchmark.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CfitsioBenchmark_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parameter_test) {

  Test::BenchmarkFactory factory;
  factory.registerBenchmark<Test::CfitsioBenchmark>("colwise", -1);
  factory.registerBenchmark<Test::CfitsioBenchmark>("optimal", 0);
  factory.registerBenchmark<Test::CfitsioBenchmark>("rowwise", 1);

  const auto colwise = dynamic_cast<Test::CfitsioBenchmark*>(factory.createBenchmark("colwise", "file.fits").get());
  const auto optimal = dynamic_cast<Test::CfitsioBenchmark*>(factory.createBenchmark("optimal", "file.fits").get());
  const auto rowwise = dynamic_cast<Test::CfitsioBenchmark*>(factory.createBenchmark("rowwise", "file.fits").get());

  BOOST_TEST(colwise->rowChunkSize() == -1);
  BOOST_TEST(optimal->rowChunkSize() == 0);
  BOOST_TEST(rowwise->rowChunkSize() == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
