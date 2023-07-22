// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Compression.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(disabled_scaling_test) {
  Scaling scale = 0;
  BOOST_TEST(not scale);
  BOOST_TEST((scale.type() == Scaling::Type::Absolute));
  BOOST_TEST((scale.value() == 0));
}

BOOST_AUTO_TEST_CASE(absolute_scaling_test) {
  Scaling scale = 8;
  BOOST_TEST(bool(scale));
  BOOST_TEST((scale.type() == Scaling::Type::Absolute));
  BOOST_TEST((scale.value() == 8));
  BOOST_CHECK_THROW((Scaling(-scale.value())), FitsError);
}

BOOST_AUTO_TEST_CASE(factor_scaling_test) {
  Scaling scale = Tile::rms * 2.5;
  BOOST_TEST(bool(scale));
  BOOST_TEST((scale.type() == Scaling::Type::Factor));
  BOOST_TEST((scale.value() == 2.5));
  BOOST_CHECK_THROW((Tile::rms * -scale.value()), FitsError);
  auto identity = scale / 2.5;
  BOOST_TEST(identity.isIdentity());
}

BOOST_AUTO_TEST_CASE(inverse_scaling_test) {
  Scaling scale = Tile::rms / 4;
  BOOST_TEST(bool(scale));
  BOOST_TEST((scale.type() == Scaling::Type::Inverse));
  BOOST_TEST((scale.value() == 4));
  BOOST_CHECK_THROW((Tile::rms / -scale.value()), FitsError);
  auto identity = scale * 4;
  BOOST_TEST(identity.isIdentity());
}

BOOST_AUTO_TEST_CASE(scaling_equality_test) {
  const auto a0 = Scaling(0);
  const auto a1 = Scaling(1);
  const auto f1 = Tile::rms * 1;
  const auto f2 = Tile::rms * 2;
  const auto f2b = Tile::rms * 2;
  const auto i1 = Tile::rms / 1;
  const auto i2 = Tile::rms / 0.5;
  BOOST_TEST((a0 != a1));
  BOOST_TEST((a1 != f1));
  BOOST_TEST((f1 != f2));
  BOOST_TEST((f2 == f2b));
  BOOST_TEST((f1 == i1));
  BOOST_TEST((f2 == i2));
}

BOOST_AUTO_TEST_CASE(default_quantization_test) {
  Quantization q;
  BOOST_TEST(not q);
  BOOST_TEST(not q.level());
  BOOST_TEST((q.dithering() == Quantization::Dithering::None));
  BOOST_CHECK_THROW(q.dithering(Quantization::Dithering::EveryPixel), FitsError); // Cannot dither disabled q
}

BOOST_AUTO_TEST_CASE(default_dithering_test) {
  Scaling level(4);
  Quantization q(level);
  BOOST_TEST(bool(q));
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Quantization::Dithering::EveryPixel));
}

BOOST_AUTO_TEST_CASE(quantization_dithering_test) {
  const auto level = Tile::rms / 4; // CFITSIO default
  Quantization q(level);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Quantization::Dithering::EveryPixel));
  q.dithering(Quantization::Dithering::NonZeroPixel);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Quantization::Dithering::NonZeroPixel));
}

BOOST_AUTO_TEST_CASE(quantization_equality_test) {
  Quantization q0;
  Quantization q0n(0, Quantization::Dithering::None);
  Quantization q3(3);
  Quantization q4(4);
  Quantization q4n(4, Quantization::Dithering::None);
  Quantization q4e(4, Quantization::Dithering::EveryPixel);
  Quantization q4nz(4, Quantization::Dithering::NonZeroPixel);
  BOOST_TEST((q0 == q0n));
  BOOST_TEST((q0 != q4n));
  BOOST_TEST((q3 != q4));
  BOOST_TEST((q4 != q4n));
  BOOST_TEST((q4 == q4e));
  BOOST_TEST((q4 != q4nz));
}

template <typename TAlgo>
void testAlgoMixinParameters(long dimension = 0) {

  Position<-1> shape(dimension);
  std::fill(shape.begin(), shape.end(), 300);
  TAlgo algo(shape);

  // verify shape of algo is correctly stored at construction
  const auto shape2 = algo.tiling();
  BOOST_TEST((shape2 == shape));

  // check default quantization values:
  BOOST_TEST((algo.quantization() == Quantization()));

  // set/get quantization:
  Quantization quantization;
  quantization.level(5);
  quantization.dithering(Quantization::Dithering::None);
  algo.quantization(quantization);
  BOOST_TEST((algo.quantization() == quantization));
}

// specific to the NoCompression algo
template <>
void testAlgoMixinParameters<NoCompression>(long) {
  NoCompression algo;
  BOOST_TEST((algo.tiling() == Position<-1>()));
}

BOOST_AUTO_TEST_CASE(algo_mixin_test) {

  testAlgoMixinParameters<NoCompression>();

  for (long n = 0; n <= 6; ++n) {
    testAlgoMixinParameters<Rice>(n);
  }

  testAlgoMixinParameters<HCompress>(2);

  for (long n = 0; n <= 6; ++n) {
    testAlgoMixinParameters<Plio>(n);
  }

  for (long n = 0; n <= 6; ++n) {
    testAlgoMixinParameters<Gzip>(n);
  }

  for (long n = 0; n <= 6; ++n) {
    testAlgoMixinParameters<ShuffledGzip>(n);
  }
}

BOOST_AUTO_TEST_CASE(hcompress_test) {

  const Position<-1>& shape {300, 200};
  HCompress algo(shape);

  // check default hcompress params values:
  BOOST_TEST((algo.scaling() == Scaling(0)));
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Scaling scale = 5;
  algo.scaling(scale);
  algo.enableSmoothing();
  BOOST_TEST((algo.scaling() == scale));
  BOOST_TEST(algo.isSmooth() == true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()