// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/MefFile.h"
#include "EleFits/Strategy.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

struct AfterOpening : Action {
  static const std::string keyword;
  static constexpr int value = 0;
  void afterOpening(const Hdu& hdu) {
    hdu.header().write(keyword, value);
  }
};

const std::string AfterOpening::keyword = "OPEN";

struct AfterAccessing : Action {
  static const std::string keyword;
  static constexpr int value = 1;
  void afterAccessing(const Hdu& hdu) {
    hdu.header().write(keyword, value);
  }
};

const std::string AfterAccessing::keyword = "ACCESS";

Strategy makeStrategy() {
  Strategy out;
  out.append(AfterOpening());
  out.append(AfterAccessing());
  return out;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Strategy_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(immediate_action_test) {
  MefFile mef("/tmp/action.fits", FileMode::Temporary, AfterOpening(), AfterAccessing());
  BOOST_TEST(mef.primary().header().has(AfterOpening::keyword));
  BOOST_TEST(mef.primary().header().has(AfterAccessing::keyword));
  const auto& table = mef.appendBintableHeader();
  BOOST_TEST(not table.header().has(AfterOpening::keyword));
  BOOST_TEST(table.header().has(AfterAccessing::keyword));
  const auto& image = mef.appendImageHeader();
  BOOST_TEST(not image.header().has(AfterOpening::keyword));
  BOOST_TEST(table.header().has(AfterAccessing::keyword));
}

BOOST_AUTO_TEST_CASE(immediate_strategy_test) {
  MefFile mef("/tmp/strategy.fits", FileMode::Temporary, makeStrategy());
  BOOST_TEST(mef.primary().header().has(AfterOpening::keyword));
  BOOST_TEST(mef.primary().header().has(AfterAccessing::keyword));
  const auto& table = mef.appendBintableHeader();
  BOOST_TEST(not table.header().has(AfterOpening::keyword));
  BOOST_TEST(table.header().has(AfterAccessing::keyword));
  const auto& image = mef.appendImageHeader();
  BOOST_TEST(not image.header().has(AfterOpening::keyword));
  BOOST_TEST(table.header().has(AfterAccessing::keyword));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
