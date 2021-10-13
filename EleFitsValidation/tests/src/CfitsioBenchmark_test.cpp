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

#include "EleFitsValidation/CfitsioBenchmark.h"

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

  // Store a reference to unique_ptr to avoid corresponding constructor to be called
  const auto colwise = factory.createBenchmark("colwise", "file.fits");
  const auto optimal = factory.createBenchmark("optimal", "file.fits");
  const auto rowwise = factory.createBenchmark("rowwise", "file.fits");

  // Cast to raw pointers
  const auto colwiseCPtr = dynamic_cast<Test::CfitsioBenchmark*>(colwise.get());
  const auto optimalPtr = dynamic_cast<Test::CfitsioBenchmark*>(optimal.get());
  const auto rowwisePtr = dynamic_cast<Test::CfitsioBenchmark*>(rowwise.get());

  BOOST_TEST(colwiseCPtr->rowChunkSize() == -1);
  BOOST_TEST(optimalPtr->rowChunkSize() == 0);
  BOOST_TEST(rowwisePtr->rowChunkSize() == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
