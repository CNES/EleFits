// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Compression.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

namespace Euclid {
namespace Fits {

// Dummy definition to please compiler, although the method is not used here
template <typename TDerived>
void AlgoMixin<TDerived>::compress(void*) const {}
// FIXME rm when the function is defined in Compression.hpp

} // namespace Fits
} // namespace Euclid

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(factor_test) {

  const float positiveFactor = 5.f;
  const float zeroFactor = 0.f;
  const float negativeFactor = -5.f;

  const Param none_f = Param::none();
  BOOST_TEST(none_f.value() == zeroFactor);
  BOOST_TEST(none_f.type() == Param::Type::None);

  const Param absolute_f = Param::absolute(positiveFactor);
  BOOST_CHECK_THROW(Param::absolute(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Param::absolute(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(absolute_f.value() == positiveFactor);
  BOOST_TEST(absolute_f.type() == Param::Type::Absolute);

  const Param relative_f = Param::relative(positiveFactor);
  BOOST_CHECK_THROW(Param::relative(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Param::relative(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(relative_f.value() == positiveFactor);
  BOOST_TEST(relative_f.type() == Param::Type::Relative);

  // testing == operator:
  const Param f1 = Param::relative(5.f);
  const Param f2 = Param::relative(5.f);
  const Param f3 = Param::absolute(5.f);
  const Param f4 = Param::relative(4.f);
  BOOST_TEST((f1 == f2));
  BOOST_TEST(not(f1 == f3));
  BOOST_TEST(not(f1 == f4));
}

BOOST_AUTO_TEST_CASE(default_quantization_test) {
  Quantization q;
  BOOST_TEST(not q);
  BOOST_TEST(not q.level());
  BOOST_TEST((q.dithering() == Dithering::None));
  BOOST_CHECK_THROW(q.dithering(Dithering::EveryPixel), FitsError); // Cannot dither disabled q
}

BOOST_AUTO_TEST_CASE(default_dithering_test) {
  const auto level = Param::absolute(4); // CFITSIO default
  Quantization q(level);
  BOOST_TEST(q);
  BOOST_TEST(q.level() == level);
  BOOST_TEST((q.dithering() == Dithering::EveryPixel));
}

BOOST_AUTO_TEST_CASE(quantization_level_test) {
  const auto level = Param::relative(4);
  Quantization q;
  q.level(level);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Dithering::None));
  q.dithering(Dithering::NonZeroPixel);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Dithering::NonZeroPixel));
}

BOOST_AUTO_TEST_CASE(quantization_equality_test) {
  Quantization q0;
  Quantization q0n(Param::none(), Dithering::None);
  Quantization q3(Param::absolute(3));
  Quantization q4(Param::absolute(4));
  Quantization q4n(Param::absolute(4), Dithering::None);
  Quantization q4e(Param::absolute(4), Dithering::EveryPixel);
  Quantization q4nz(Param::absolute(4), Dithering::NonZeroPixel);
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
  quantization.level(Param::absolute(5));
  quantization.dithering(Dithering::None);
  algo.quantization(quantization);
  BOOST_TEST((algo.quantization() == quantization));
}

// specific to the NoCompression algo
template <>
void testAlgoMixinParameters<NoCompression>(long) {

  NoCompression algo;

  // verify shape of algo is correctly stored at construction
  BOOST_TEST((algo.tiling() == Position<-1>()));
}

BOOST_AUTO_TEST_CASE(algo_mixin_test) {

  testAlgoMixinParameters<NoCompression>();

  testAlgoMixinParameters<Rice>(0);
  testAlgoMixinParameters<Rice>(1);
  testAlgoMixinParameters<Rice>(2);
  testAlgoMixinParameters<Rice>(3);
  testAlgoMixinParameters<Rice>(4);
  testAlgoMixinParameters<Rice>(5);
  testAlgoMixinParameters<Rice>(6);

  testAlgoMixinParameters<HCompress>(2);

  testAlgoMixinParameters<Plio>(0);
  testAlgoMixinParameters<Plio>(1);
  testAlgoMixinParameters<Plio>(2);
  testAlgoMixinParameters<Plio>(3);
  testAlgoMixinParameters<Plio>(4);
  testAlgoMixinParameters<Plio>(5);
  testAlgoMixinParameters<Plio>(6);

  testAlgoMixinParameters<Gzip>(0);
  testAlgoMixinParameters<Gzip>(1);
  testAlgoMixinParameters<Gzip>(2);
  testAlgoMixinParameters<Gzip>(3);
  testAlgoMixinParameters<Gzip>(4);
  testAlgoMixinParameters<Gzip>(5);
  testAlgoMixinParameters<Gzip>(6);

  testAlgoMixinParameters<ShuffledGzip>(0);
  testAlgoMixinParameters<ShuffledGzip>(1);
  testAlgoMixinParameters<ShuffledGzip>(2);
  testAlgoMixinParameters<ShuffledGzip>(3);
  testAlgoMixinParameters<ShuffledGzip>(4);
  testAlgoMixinParameters<ShuffledGzip>(5);
  testAlgoMixinParameters<ShuffledGzip>(6);
}

BOOST_AUTO_TEST_CASE(hcompress_test) {

  const Position<-1>& shape {300, 200};
  HCompress algo(shape);

  // check default hcompress params values:
  BOOST_TEST((algo.scale() == Param::none()));
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Param scale = Param::absolute(5.f);
  algo.scale(scale);
  algo.enableSmoothing();
  BOOST_TEST((algo.scale() == scale));
  BOOST_TEST(algo.isSmooth() == true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()