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

#include "EL_FitsData/DataUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

template <typename T>
struct PassBySpy {

  PassBySpy(T v) : value(v), moved(false), copied(false) {}

  PassBySpy(const PassBySpy& p) : value(p.value), moved(false), copied(true) {}

  PassBySpy(PassBySpy&& p) : value(p.value), moved(true), copied(false) {}

  ~PassBySpy() = default;

  PassBySpy& operator=(const PassBySpy& p) {
    value = p.value;
    copied = true;
  }

  PassBySpy& operator=(PassBySpy&& p) {
    value = p.value;
    moved = true;
  }

  T value;
  bool moved;
  bool copied;
};

struct Body {
  std::string name;
  int age;
  float height;
  float mass;
  float bmi() const {
    return mass / (height * height);
  }
};

std::string toString(std::string name, int age, float height, float mass) {
  return name + " (" + std::to_string(age) + ") :" + std::to_string(height) + "m, " + std::to_string(mass) + "kg";
}

template <typename TSeq>
void dispatchSeq(TSeq&& seq, bool isTuple);

template <typename T>
void dispatchSeq(const std::vector<T>& seq, bool isTuple);

template <typename T>
void dispatchSeq(std::vector<T>&& seq, bool isTuple);

template <typename TSeq>
void dispatchSeq(TSeq&& seq, bool isTuple) {
  seqForeach(std::forward<TSeq>(seq), [&](const auto& e) {
    (void)e;
    BOOST_TEST(isTuple);
  });
}

template <typename T>
void dispatchSeq(const std::vector<T>& seq, bool isTuple) {
  for (const auto& e : seq) {
    (void)e;
    BOOST_TEST(not isTuple);
  }
}

template <typename T>
void dispatchSeq(std::vector<T>&& seq, bool isTuple) {
  for (const auto& e : seq) {
    (void)e;
    BOOST_TEST(not isTuple);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(DataUtils_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(typed_test) {
  const std::string name = "TOTOTATATITI";
  const long index = 707074747171;
  BOOST_TEST(Named<int>(name).name == name);
  BOOST_TEST(Indexed<int>(index).index == index);
}

BOOST_AUTO_TEST_CASE(tuple_as_test) {
  const std::tuple<std::string, int, float, float> tuple { "TODO", 20, 1.8, 75 };
  const auto body = tupleAs<Body>(tuple);
  BOOST_TEST(body.name == "TODO");
  BOOST_TEST(body.age == 20);
  BOOST_TEST(body.height > 1.75);
  BOOST_TEST(body.height < 1.85);
  BOOST_TEST(body.mass == 75);
  BOOST_TEST(body.bmi() < 30); // TODO relevant?
}

BOOST_AUTO_TEST_CASE(tuple_apply_test) {
  std::tuple<std::string, int, float, float> guy { "GUY", 18, 1.7, 55 };
  const auto repr = tupleApply(guy, toString);
  BOOST_TEST(not repr.empty());
}

BOOST_AUTO_TEST_CASE(tuple_transform_test) {
  std::tuple<std::string, int, float, float> jo { "JO", 40, 1.6, 85 };
  auto twice = [](const auto& e) {
    return e + e;
  };
  const auto jojo = seqTransform<Body>(jo, twice);
  BOOST_TEST(jojo.name == "JOJO");
  BOOST_TEST(jojo.age > std::get<1>(jo));
  BOOST_TEST(jojo.height > std::get<2>(jo));
  BOOST_TEST(jojo.mass > std::get<3>(jo));
}

BOOST_AUTO_TEST_CASE(tuple_foreach_test) {
  std::tuple<std::string, int, float, float> me { "ME", 32, 1.75, 65 };
  auto twice = [](auto& e) {
    e += e;
  };
  seqForeach(me, twice);
  BOOST_TEST(std::get<0>(me) == "MEME");
  BOOST_TEST(std::get<1>(me) > 32);
  BOOST_TEST(std::get<2>(me) > 1.75);
  BOOST_TEST(std::get<3>(me) > 65);
}

BOOST_AUTO_TEST_CASE(seq_dispatch_test) {
  const std::tuple<int, float> t { 1, 3.14 };
  const std::vector<int> v { 1, 2 };
  dispatchSeq(t, true);
  dispatchSeq(v, false);
  dispatchSeq(std::vector<float> { 1, 3.14 }, false);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
