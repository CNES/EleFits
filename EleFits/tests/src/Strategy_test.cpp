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
  void opened(const Hdu& hdu) {
    hdu.header().write(keyword, value);
  }
};

const std::string AfterOpening::keyword = "OPENED";

struct AfterAccessing : Action {
  static const std::string keyword;
  static constexpr int value = 1;
  void accessed(const Hdu& hdu) {
    hdu.header().write(keyword, value);
  }
};

const std::string AfterAccessing::keyword = "ACCESSED";

struct AfterCreating : Action {
  static const std::string keyword;
  static constexpr int value = 2;
  void created(const Hdu& hdu) {
    hdu.header().write(keyword, value);
  }
};

const std::string AfterCreating::keyword = "CREATED";

Strategy make_strategy() {
  Strategy out;
  out.append(AfterOpening());
  out.append(AfterAccessing());
  out.append(AfterCreating());
  return out;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Strategy_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctor_actions_test) {

  MefFile mef("/tmp/action.fits", FileMode::Temporary, AfterOpening(), AfterAccessing(), AfterCreating());
  const auto& primary = mef.primary().header();
  const auto& table = mef.appendBintableHeader().header();
  const auto& image = mef.appendImageHeader().header();
  const auto& accessed = mef[1].header();
  Test::TemporaryMefFile src;
  const auto& copied = mef.appendCopy(src.primary()).header();

  BOOST_TEST(primary.has(AfterOpening::keyword));
  BOOST_TEST(primary.has(AfterAccessing::keyword));
  BOOST_TEST(not primary.has(AfterCreating::keyword)); // FIXME is it what we want?

  BOOST_TEST(not table.has(AfterOpening::keyword));
  BOOST_TEST(not table.has(AfterAccessing::keyword));
  BOOST_TEST(table.has(AfterCreating::keyword));

  BOOST_TEST(not image.has(AfterOpening::keyword));
  BOOST_TEST(not image.has(AfterAccessing::keyword));
  BOOST_TEST(image.has(AfterCreating::keyword));

  BOOST_TEST(not accessed.has(AfterAccessing::keyword));

  BOOST_TEST(not copied.has(AfterOpening::keyword));
  BOOST_TEST(copied.has(AfterAccessing::keyword));
  BOOST_TEST(not copied.has(AfterCreating::keyword));
}

BOOST_AUTO_TEST_CASE(ctor_strategy_test) {

  MefFile mef("/tmp/strategy.fits", FileMode::Temporary, make_strategy());
  const auto& primary = mef.primary().header();
  const auto& table = mef.appendBintableHeader().header();
  const auto& image = mef.appendImageHeader().header();
  const auto& accessed = mef[1].header();
  Test::TemporaryMefFile src;
  const auto& copied = mef.appendCopy(src.primary()).header();

  BOOST_TEST(primary.has(AfterOpening::keyword));
  BOOST_TEST(primary.has(AfterAccessing::keyword));
  BOOST_TEST(not primary.has(AfterCreating::keyword)); // FIXME is it what we want?

  BOOST_TEST(not table.has(AfterOpening::keyword));
  BOOST_TEST(not table.has(AfterAccessing::keyword));
  BOOST_TEST(table.has(AfterCreating::keyword));

  BOOST_TEST(not image.has(AfterOpening::keyword));
  BOOST_TEST(not image.has(AfterAccessing::keyword));
  BOOST_TEST(image.has(AfterCreating::keyword));

  BOOST_TEST(not accessed.has(AfterAccessing::keyword));

  BOOST_TEST(not copied.has(AfterOpening::keyword));
  BOOST_TEST(copied.has(AfterAccessing::keyword));
  BOOST_TEST(not copied.has(AfterCreating::keyword));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
