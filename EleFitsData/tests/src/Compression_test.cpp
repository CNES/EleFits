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

BOOST_AUTO_TEST_CASE(disabled_scaling_test) {
  Compression::Scaling scale = 0;
  BOOST_TEST(not scale);
  BOOST_TEST((scale.type() == Compression::Scaling::Type::Absolute));
  BOOST_TEST((scale.value() == 0));
}

BOOST_AUTO_TEST_CASE(absolute_scaling_test) {
  Compression::Scaling scale = 8;
  BOOST_TEST(bool(scale));
  BOOST_TEST((scale.type() == Compression::Scaling::Type::Absolute));
  BOOST_TEST((scale.value() == 8));
  BOOST_CHECK_THROW((Compression::Scaling(-scale.value())), FitsError);
}

BOOST_AUTO_TEST_CASE(factor_scaling_test) {
  Compression::Scaling scale = Compression::rms * 2.5;
  BOOST_TEST(bool(scale));
  BOOST_TEST((scale.type() == Compression::Scaling::Type::Factor));
  BOOST_TEST((scale.value() == 2.5));
  BOOST_CHECK_THROW((Compression::rms * -scale.value()), FitsError);
}

BOOST_AUTO_TEST_CASE(inverse_scaling_test) {
  Compression::Scaling scale = Compression::rms / 4;
  BOOST_TEST(bool(scale));
  BOOST_TEST((scale.type() == Compression::Scaling::Type::Inverse));
  BOOST_TEST((scale.value() == 4));
  BOOST_CHECK_THROW((Compression::rms / -scale.value()), FitsError);
}

BOOST_AUTO_TEST_CASE(scaling_equality_test) {
  const auto a0 = Compression::Scaling(0);
  const auto a1 = Compression::Scaling(1);
  const auto f1 = Compression::rms * 1;
  const auto f2 = Compression::rms * 2;
  const auto f2b = Compression::rms * 2;
  const auto i1 = Compression::rms / 1;
  const auto i2 = Compression::rms / 0.5;
  BOOST_TEST((a0 != a1));
  BOOST_TEST((a1 != f1));
  BOOST_TEST((f1 != f2));
  BOOST_TEST((f2 == f2b));
  BOOST_TEST((f1 == i1));
  BOOST_TEST((f2 == i2));
}

BOOST_AUTO_TEST_CASE(default_quantization_test) {
  Compression::Quantization q;
  BOOST_TEST(not q);
  BOOST_TEST(not q.level());
  BOOST_TEST((q.dithering() == Compression::Dithering::None));
  BOOST_CHECK_THROW(q.dithering(Compression::Dithering::EveryPixel), FitsError); // Cannot dither disabled q
}

BOOST_AUTO_TEST_CASE(default_dithering_test) {
  Compression::Scaling level(4);
  Compression::Quantization q(level);
  BOOST_TEST(bool(q));
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Compression::Dithering::EveryPixel));
}

BOOST_AUTO_TEST_CASE(quantization_dithering_test) {
  const auto level = Compression::rms / 4; // CFITSIO default
  Compression::Quantization q(level);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Compression::Dithering::EveryPixel));
  q.dithering(Compression::Dithering::NonZeroPixel);
  BOOST_TEST((q.level() == level));
  BOOST_TEST((q.dithering() == Compression::Dithering::NonZeroPixel));
}

BOOST_AUTO_TEST_CASE(quantization_equality_test) {
  Compression::Quantization q0;
  Compression::Quantization q0n(0, Compression::Dithering::None);
  Compression::Quantization q3(3);
  Compression::Quantization q4(4);
  Compression::Quantization q4n(4, Compression::Dithering::None);
  Compression::Quantization q4e(4, Compression::Dithering::EveryPixel);
  Compression::Quantization q4nz(4, Compression::Dithering::NonZeroPixel);
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
  BOOST_TEST((algo.quantization() == Compression::Quantization()));

  // set/get quantization:
  Compression::Quantization quantization;
  quantization.level(5);
  quantization.dithering(Compression::Dithering::None);
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
  BOOST_TEST((algo.scaling() == Compression::Scaling(0)));
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Compression::Scaling scale = 5;
  algo.scaling(scale);
  algo.enableSmoothing();
  BOOST_TEST((algo.scaling() == scale));
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