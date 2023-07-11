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

} // namespace Fits
} // namespace Euclid

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(param_type_test) {

  const double positiveValue = 5;
  const double zeroValue = 0;
  const double negativeValue = -5;

  const auto none = Param::none();
  BOOST_TEST(none.value() == zeroValue);
  BOOST_TEST(none.type() == Param::Type::None);

  const auto absolute = Param::absolute(positiveValue);
  BOOST_CHECK_THROW(Param::absolute(zeroValue), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Param::absolute(negativeValue), Euclid::Fits::FitsError);

  BOOST_TEST(absolute.value() == positiveValue);
  BOOST_TEST(absolute.type() == Param::Type::Absolute);

  const auto relative = Param::relative(positiveValue);
  BOOST_CHECK_THROW(Param::relative(zeroValue), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Param::relative(negativeValue), Euclid::Fits::FitsError);

  BOOST_TEST(relative.value() == positiveValue);
  BOOST_TEST(relative.type() == Param::Type::Relative);
}

BOOST_AUTO_TEST_CASE(param_equality_test) {
  const auto r4 = Param::relative(4);
  const auto r5 = Param::relative(5);
  const auto r5b = Param::relative(5);
  const auto a5 = Param::absolute(5);
  BOOST_TEST((r5 == r5b));
  BOOST_TEST((r5 != a5));
  BOOST_TEST((r5 != r4));
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
  BOOST_TEST((algo.scale() == Param::none()));
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Param scale = Param::absolute(5);
  algo.scale(scale);
  algo.enableSmoothing();
  BOOST_TEST((algo.scale() == scale));
  BOOST_TEST(algo.isSmooth() == true);
}

BOOST_AUTO_TEST_CASE(algo_maker_test) {
  for (const auto& bitpix : {-64, -32, 8, 16, 32, 64}) {
    for (long n = 1; n <= 6; ++n) {
      const auto algo = Compression::makeAlgo(bitpix, n);
      const auto lossless = Compression::makeLosslessAlgo(bitpix, n);
      BOOST_CHECK_THROW(dynamic_cast<const NoCompression&>(*algo), std::bad_cast);
      BOOST_CHECK_THROW(dynamic_cast<const NoCompression&>(*lossless), std::bad_cast);
      BOOST_TEST(lossless->isLossless());
    }
  }
}

template <typename T, long N>
void checkRasterAlgoMaker(T value) {

  Position<N> shape(N == -1 ? 2 : N);
  std::fill(shape.begin(), shape.end(), 2);

  VecRaster<T, N> raster(std::move(shape));
  std::fill(raster.begin(), raster.end(), value);

  const auto algo = Compression::makeAlgo(raster);
  const auto lossless = Compression::makeLosslessAlgo(raster);

  if (N == 0) {
    BOOST_CHECK_NO_THROW(dynamic_cast<const NoCompression&>(*algo));
    BOOST_CHECK_NO_THROW(dynamic_cast<const NoCompression&>(*lossless));
  } else if (std::is_integral_v<T> && value < std::pow(2, 24)) {
    BOOST_CHECK_NO_THROW(dynamic_cast<const Plio&>(*algo));
    BOOST_CHECK_NO_THROW(dynamic_cast<const Plio&>(*lossless));
  } else {
    BOOST_CHECK_THROW(dynamic_cast<const Plio&>(*algo), std::bad_cast);
    BOOST_CHECK_THROW(dynamic_cast<const Plio&>(*lossless), std::bad_cast);
    BOOST_CHECK_THROW(dynamic_cast<const NoCompression&>(*algo), std::bad_cast);
    BOOST_CHECK_THROW(dynamic_cast<const NoCompression&>(*lossless), std::bad_cast);
    BOOST_TEST(lossless->isLossless());
  }
}

template <typename T, long N>
void checkLimitRastersAlgoMaker() {
  checkRasterAlgoMaker<T, N>(1);
  checkRasterAlgoMaker<T, N>(std::numeric_limits<T>::max());
}

BOOST_AUTO_TEST_CASE(raster_algo_maker_test) {

  checkLimitRastersAlgoMaker<char, -1>();
  checkLimitRastersAlgoMaker<char, 0>();
  checkLimitRastersAlgoMaker<char, 1>();
  checkLimitRastersAlgoMaker<char, 2>();

  checkLimitRastersAlgoMaker<short, -1>();
  checkLimitRastersAlgoMaker<short, 0>();
  checkLimitRastersAlgoMaker<short, 1>();
  checkLimitRastersAlgoMaker<short, 2>();

  checkLimitRastersAlgoMaker<int, -1>();
  checkLimitRastersAlgoMaker<int, 0>();
  checkLimitRastersAlgoMaker<int, 1>();
  checkLimitRastersAlgoMaker<int, 2>();

  checkLimitRastersAlgoMaker<long, -1>();
  checkLimitRastersAlgoMaker<long, 0>();
  checkLimitRastersAlgoMaker<long, 1>();
  checkLimitRastersAlgoMaker<long, 2>();

  checkLimitRastersAlgoMaker<long long, -1>();
  checkLimitRastersAlgoMaker<long long, 0>();
  checkLimitRastersAlgoMaker<long long, 1>();
  checkLimitRastersAlgoMaker<long long, 2>();

  checkLimitRastersAlgoMaker<float, -1>();
  checkLimitRastersAlgoMaker<float, 0>();
  checkLimitRastersAlgoMaker<float, 1>();
  checkLimitRastersAlgoMaker<float, 2>();

  checkLimitRastersAlgoMaker<double, -1>();
  checkLimitRastersAlgoMaker<double, 0>();
  checkLimitRastersAlgoMaker<double, 1>();
  checkLimitRastersAlgoMaker<double, 2>();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()