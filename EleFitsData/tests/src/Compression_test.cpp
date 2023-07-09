// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Compression.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

namespace Euclid {
namespace Fits {
namespace Compression {

// Dummy definition to please compiler, although the method is not used here
template <typename TDerived>
void AlgoMixin<TDerived>::compress(void*) const {}
// FIXME rm when the function is defined in Compression.hpp

} // namespace Compression
} // namespace Fits
} // namespace Euclid

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(factor_test) {

  const float positiveFactor = 5.f;
  const float zeroFactor = 0.f;
  const float negativeFactor = -5.f;

  const Compression::Param none_f = Compression::Param::none();
  BOOST_TEST(none_f.value() == zeroFactor);
  BOOST_TEST(none_f.type() == Compression::Param::Type::None);

  const Compression::Param absolute_f = Compression::Param::absolute(positiveFactor);
  BOOST_CHECK_THROW(Compression::Param::absolute(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Compression::Param::absolute(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(absolute_f.value() == positiveFactor);
  BOOST_TEST(absolute_f.type() == Compression::Param::Type::Absolute);

  const Compression::Param relative_f = Compression::Param::relative(positiveFactor);
  BOOST_CHECK_THROW(Compression::Param::relative(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Compression::Param::relative(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(relative_f.value() == positiveFactor);
  BOOST_TEST(relative_f.type() == Compression::Param::Type::Relative);

  // testing == operator:
  const Compression::Param f1 = Compression::Param::relative(5.f);
  const Compression::Param f2 = Compression::Param::relative(5.f);
  const Compression::Param f3 = Compression::Param::absolute(5.f);
  const Compression::Param f4 = Compression::Param::relative(4.f);
  BOOST_TEST((f1 == f2));
  BOOST_TEST(not(f1 == f3));
  BOOST_TEST(not(f1 == f4));
}

BOOST_AUTO_TEST_CASE(default_quantization_test) {
  Compression::Quantization q;
  BOOST_TEST(not q);
  BOOST_TEST(not q.level());
  BOOST_TEST((q.dithering() == Compression::Dithering::None));
  BOOST_CHECK_THROW(q.dithering(Compression::Dithering::EveryPixel), FitsError); // Cannot dither disabled q
}

BOOST_AUTO_TEST_CASE(default_dithering_test) {
  const auto level = Compression::Param::absolute(4); // CFITSIO default
  Compression::Quantization q(level);
  BOOST_TEST(q);
  BOOST_TEST(q.level() == level);
  BOOST_TEST((q.dithering() == Compression::Dithering::EveryPixel));
}

BOOST_AUTO_TEST_CASE(quantization_level_test) {
  const auto level = Compression::Param::relative(4);
  Compression::Quantization q;
  q.level(level);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Compression::Dithering::None));
  q.dithering(Compression::Dithering::NonZeroPixel);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Compression::Dithering::NonZeroPixel));
}

BOOST_AUTO_TEST_CASE(quantization_equality_test) {
  Compression::Quantization q0;
  Compression::Quantization q0n(Compression::Param::none(), Compression::Dithering::None);
  Compression::Quantization q3(Compression::Param::absolute(3));
  Compression::Quantization q4(Compression::Param::absolute(4));
  Compression::Quantization q4n(Compression::Param::absolute(4), Compression::Dithering::None);
  Compression::Quantization q4e(Compression::Param::absolute(4), Compression::Dithering::EveryPixel);
  Compression::Quantization q4nz(Compression::Param::absolute(4), Compression::Dithering::NonZeroPixel);
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
  const auto shape2 = algo.shape();
  BOOST_TEST((shape2 == shape));

  // check default quantization values:
  BOOST_TEST((algo.quantization() == Compression::Quantization()));

  // set/get quantization:
  Compression::Quantization quantization;
  quantization.level(Compression::Param::absolute(5));
  quantization.dithering(Compression::Dithering::None);
  algo.quantization(quantization);
  BOOST_TEST((algo.quantization() == quantization));
}

// specific to the None algo
template <>
void testAlgoMixinParameters<Compression::None>(long) {

  Compression::None algo;

  // verify shape of algo is correctly stored at construction
  BOOST_TEST((algo.shape() == Position<-1>()));
}

BOOST_AUTO_TEST_CASE(algo_mixin_test) {

  testAlgoMixinParameters<Compression::None>();

  testAlgoMixinParameters<Compression::Rice>(0);
  testAlgoMixinParameters<Compression::Rice>(1);
  testAlgoMixinParameters<Compression::Rice>(2);
  testAlgoMixinParameters<Compression::Rice>(3);
  testAlgoMixinParameters<Compression::Rice>(4);
  testAlgoMixinParameters<Compression::Rice>(5);
  testAlgoMixinParameters<Compression::Rice>(6);

  testAlgoMixinParameters<Compression::HCompress>(2);

  testAlgoMixinParameters<Compression::Plio>(0);
  testAlgoMixinParameters<Compression::Plio>(1);
  testAlgoMixinParameters<Compression::Plio>(2);
  testAlgoMixinParameters<Compression::Plio>(3);
  testAlgoMixinParameters<Compression::Plio>(4);
  testAlgoMixinParameters<Compression::Plio>(5);
  testAlgoMixinParameters<Compression::Plio>(6);

  testAlgoMixinParameters<Compression::Gzip>(0);
  testAlgoMixinParameters<Compression::Gzip>(1);
  testAlgoMixinParameters<Compression::Gzip>(2);
  testAlgoMixinParameters<Compression::Gzip>(3);
  testAlgoMixinParameters<Compression::Gzip>(4);
  testAlgoMixinParameters<Compression::Gzip>(5);
  testAlgoMixinParameters<Compression::Gzip>(6);

  testAlgoMixinParameters<Compression::ShuffledGzip>(0);
  testAlgoMixinParameters<Compression::ShuffledGzip>(1);
  testAlgoMixinParameters<Compression::ShuffledGzip>(2);
  testAlgoMixinParameters<Compression::ShuffledGzip>(3);
  testAlgoMixinParameters<Compression::ShuffledGzip>(4);
  testAlgoMixinParameters<Compression::ShuffledGzip>(5);
  testAlgoMixinParameters<Compression::ShuffledGzip>(6);
}

BOOST_AUTO_TEST_CASE(hcompress_test) {

  const Position<-1>& shape {300, 200};
  Compression::HCompress algo(shape);

  // check default hcompress params values:
  BOOST_TEST((algo.scale() == Compression::Param::none()));
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Compression::Param scale = Compression::Param::absolute(5.f);
  algo.scale(scale);
  algo.enableSmoothing();
  BOOST_TEST((algo.scale() == scale));
  BOOST_TEST(algo.isSmooth() == true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()