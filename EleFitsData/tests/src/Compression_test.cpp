// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Compression.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(factor_test) {

  const float positiveFactor = 5.f;
  const float zeroFactor = 0.f;
  const float negativeFactor = -5.f;

  const Compression::Factor none_f = Compression::Factor::none();
  BOOST_TEST(none_f.value() == zeroFactor);
  BOOST_TEST(none_f.type() == Compression::Factor::Type::None);

  const Compression::Factor absolute_f = Compression::Factor::absolute(positiveFactor);
  BOOST_CHECK_THROW(Compression::Factor::absolute(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Compression::Factor::absolute(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(absolute_f.value() == positiveFactor);
  BOOST_TEST(absolute_f.type() == Compression::Factor::Type::Absolute);

  const Compression::Factor relative_f = Compression::Factor::relative(positiveFactor);
  BOOST_CHECK_THROW(Compression::Factor::relative(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Compression::Factor::relative(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(relative_f.value() == positiveFactor);
  BOOST_TEST(relative_f.type() == Compression::Factor::Type::Relative);

  // testing == operator:
  const Compression::Factor f1 = Compression::Factor::relative(5.f);
  const Compression::Factor f2 = Compression::Factor::relative(5.f);
  const Compression::Factor f3 = Compression::Factor::absolute(5.f);
  const Compression::Factor f4 = Compression::Factor::relative(4.f);
  BOOST_TEST((f1 == f2));
  BOOST_TEST(not(f1 == f3));
  BOOST_TEST(not(f1 == f4));
}

BOOST_AUTO_TEST_CASE(quantization_test) {

  Compression::Quantization quant;

  // default values:
  BOOST_TEST((quant.level() == Compression::Factor::none()));
  BOOST_TEST(quant.hasLossyInt() == false);
  BOOST_TEST((quant.dithering() == Compression::Dithering::EveryPixel));

  // setting quantization level:
  const float positiveLevel = 5.f;

  quant.level(Compression::Factor::absolute(positiveLevel));
  BOOST_TEST((quant.level() == Compression::Factor::absolute(positiveLevel)));

  quant.level(Compression::Factor::relative(positiveLevel));
  BOOST_TEST((quant.level() == Compression::Factor::relative(positiveLevel)));

  quant.level(Compression::Factor::none());
  BOOST_TEST((quant.level() == Compression::Factor::none()));

  // turning on/off lossyInt:
  quant.enableLossyInt();
  BOOST_TEST(quant.hasLossyInt() == true);
  quant.disableLossyInt();
  BOOST_TEST(quant.hasLossyInt() == false);

  // setting dithering:
  quant.dithering(Compression::Dithering::None);
  BOOST_TEST((quant.dithering() == Compression::Dithering::None));

  quant.dithering(Compression::Dithering::NonZeroPixel);
  BOOST_TEST((quant.dithering() == Compression::Dithering::NonZeroPixel));

  quant.dithering(Compression::Dithering::EveryPixel);
  BOOST_TEST((quant.dithering() == Compression::Dithering::EveryPixel));

  // verify that chaining setters works:
  quant.level(Compression::Factor::relative(positiveLevel)).enableLossyInt().dithering(Compression::Dithering::None);
  BOOST_TEST(quant.level().value() == positiveLevel);
  BOOST_TEST(quant.level().type() == Compression::Factor::Type::Relative);
  BOOST_TEST(quant.hasLossyInt() == true);
  BOOST_TEST((quant.dithering() == Compression::Dithering::None));

  // testing == operator:
  Compression::Quantization q1, q2, q3, q4, q5;
  q1.level(Compression::Factor::absolute(3.f)).enableLossyInt().dithering(Compression::Dithering::EveryPixel);
  q2.level(Compression::Factor::absolute(3.f)).enableLossyInt().dithering(Compression::Dithering::EveryPixel);
  q3.level(Compression::Factor::absolute(4.f)).enableLossyInt().dithering(Compression::Dithering::EveryPixel);
  q4.level(Compression::Factor::absolute(3.f)).disableLossyInt().dithering(Compression::Dithering::EveryPixel);
  q5.level(Compression::Factor::absolute(3.f)).enableLossyInt().dithering(Compression::Dithering::None);
  BOOST_TEST((q1 == q2));
  BOOST_TEST(not(q1 == q3));
  BOOST_TEST(not(q1 == q4));
  BOOST_TEST(not(q1 == q5));
}

template <typename TAlgo, long NDim>
void testAlgoMixinParameters() {

  Position<NDim> shape;
  for (int i = 0; i < NDim; i++) {
    shape[i] = 300;
  }
  TAlgo algo(shape);

  // verify shape of algo is correctly stored at construction
  const auto shape2 = algo.shape();
  BOOST_TEST((shape2 == shape));

  // check default quantization values:
  BOOST_TEST(
      (algo.quantize() ==
       Compression::Quantization()
           .level(Compression::Factor::none()) // FIXME: may be changed depending on algorithm (float algos)
           .disableLossyInt()
           .dithering(Compression::Dithering::EveryPixel)));

  // set/get quantization:
  Compression::Quantization quant;
  quant.level(Compression::Factor::absolute(5.f));
  quant.enableLossyInt();
  quant.dithering(Compression::Dithering::None);
  algo.quantize(quant);
  BOOST_TEST((algo.quantize() == quant));
}

// specific to the None algo
template <typename TAlgo>
void testAlgoMixinParameters() {

  TAlgo algo;

  // verify shape of algo is correctly stored at construction
  BOOST_TEST((algo.shape() == Position<0>()));
}

BOOST_AUTO_TEST_CASE(algo_mixin_test) {

  testAlgoMixinParameters<Compression::None>();

  testAlgoMixinParameters<Compression::Rice<0>, 0>();
  testAlgoMixinParameters<Compression::Rice<1>, 1>();
  testAlgoMixinParameters<Compression::Rice<2>, 2>();
  testAlgoMixinParameters<Compression::Rice<3>, 3>();
  testAlgoMixinParameters<Compression::Rice<4>, 4>();
  testAlgoMixinParameters<Compression::Rice<5>, 5>();
  testAlgoMixinParameters<Compression::Rice<6>, 6>();

  testAlgoMixinParameters<Compression::HCompress, 2>();

  testAlgoMixinParameters<Compression::Plio<0>, 0>();
  testAlgoMixinParameters<Compression::Plio<1>, 1>();
  testAlgoMixinParameters<Compression::Plio<2>, 2>();
  testAlgoMixinParameters<Compression::Plio<3>, 3>();
  testAlgoMixinParameters<Compression::Plio<4>, 4>();
  testAlgoMixinParameters<Compression::Plio<5>, 5>();
  testAlgoMixinParameters<Compression::Plio<6>, 6>();

  testAlgoMixinParameters<Compression::Gzip<0>, 0>();
  testAlgoMixinParameters<Compression::Gzip<1>, 1>();
  testAlgoMixinParameters<Compression::Gzip<2>, 2>();
  testAlgoMixinParameters<Compression::Gzip<3>, 3>();
  testAlgoMixinParameters<Compression::Gzip<4>, 4>();
  testAlgoMixinParameters<Compression::Gzip<5>, 5>();
  testAlgoMixinParameters<Compression::Gzip<6>, 6>();

  testAlgoMixinParameters<Compression::ShuffledGzip<0>, 0>();
  testAlgoMixinParameters<Compression::ShuffledGzip<1>, 1>();
  testAlgoMixinParameters<Compression::ShuffledGzip<2>, 2>();
  testAlgoMixinParameters<Compression::ShuffledGzip<3>, 3>();
  testAlgoMixinParameters<Compression::ShuffledGzip<4>, 4>();
  testAlgoMixinParameters<Compression::ShuffledGzip<5>, 5>();
  testAlgoMixinParameters<Compression::ShuffledGzip<6>, 6>();
}

BOOST_AUTO_TEST_CASE(hcompress_test) {

  const Position<2>& shape {300, 200};
  Compression::HCompress algo(shape);

  // check default hcompress params values:
  BOOST_TEST((algo.scale() == Compression::Factor::none()));
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Compression::Factor scale = Compression::Factor::absolute(5.f);
  algo.scale(scale);
  algo.enableSmoothing();
  BOOST_TEST((algo.scale() == scale));
  BOOST_TEST(algo.isSmooth() == true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()