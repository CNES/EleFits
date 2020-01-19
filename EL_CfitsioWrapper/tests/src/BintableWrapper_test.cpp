/**
 * @file tests/src/BintableWrapper_test.cpp
 * @date 07/27/19
 * @author user
 *
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

#include <boost/test/unit_test.hpp>

#include "EL_FitsData/FitsDataFixture.h"

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/CfitsioFixture.h"

using namespace Euclid;
using namespace Cfitsio;

template<typename T>
void check_equal_vectors(const std::vector<T>& test, const std::vector<T>& expected) {
  BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), expected.begin(), expected.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BintableWrapper_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_scalar() {
  FitsIO::Test::RandomScalarColumn<T> input;
  FitsIO::Test::MinimalFile file;
  try {
    Hdu::create_bintable_extension(file.fptr, "BINEXT", input);
    const auto output = Bintable::read_column<T>(file.fptr, input.info.name);
    check_equal_vectors(output.vector(), input.vector());
  } catch(const CfitsioError& e) {
    std::cerr << "Input:" << std::endl;
    for(const auto& v : input.vector())
      std::cerr << v << ' ';
    std::cerr << std::endl;
    if(e.status == NUM_OVERFLOW)
      BOOST_WARN(e.what());
    else
      BOOST_FAIL(e.what());
  }
  
}

#define TEST_SCALAR_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE( name##_test ) { check_scalar<type>(); }

#define TEST_SCALAR(type) \
  TEST_SCALAR_ALIAS(type, type)

#define TEST_SCALAR_UNSIGNED(type) \
  TEST_SCALAR_ALIAS(unsigned type, u##type)

// TEST_SCALAR(bool) //TODO won't compile because a vector of bools has no .data()
TEST_SCALAR(char)
TEST_SCALAR(short)
TEST_SCALAR(int)
TEST_SCALAR(long)
TEST_SCALAR(float)
TEST_SCALAR(double)
TEST_SCALAR_ALIAS(std::string, string)
TEST_SCALAR_UNSIGNED(char)
TEST_SCALAR_UNSIGNED(short)
TEST_SCALAR_UNSIGNED(int)
// TEST_SCALAR_UNSIGNED(long)

template<typename T>
void check_vector() {
  FitsIO::Test::SmallVectorColumn<T> input;
  FitsIO::Test::MinimalFile file;
  Hdu::create_bintable_extension(file.fptr, "BINEXT", input);
  const auto output = Bintable::read_column<std::vector<T>>(file.fptr, input.info.name);
  const auto size = input.rows();
  BOOST_CHECK_EQUAL(output.rows(), size);
  for(std::size_t i=0; i<size; ++i)
    check_equal_vectors(output.vector()[i], input.vector()[i]);
}

#define TEST_VECTOR(type) \
  BOOST_AUTO_TEST_CASE( vector_##type##_test ) { check_vector<type>(); }

#define TEST_VECTOR_UNSIGNED(type) \
  BOOST_AUTO_TEST_CASE( vector_u##type##_test ) { check_vector<unsigned type>(); }

/*
// TEST_VECTOR(bool) //TODO won't compile because of vector specialization for bool
TEST_VECTOR(char)
TEST_VECTOR(short)
TEST_VECTOR(int)
TEST_VECTOR(long)
TEST_VECTOR(float)
TEST_VECTOR(double)
TEST_VECTOR_UNSIGNED(char)
TEST_VECTOR_UNSIGNED(short)
TEST_VECTOR_UNSIGNED(int)
TEST_VECTOR_UNSIGNED(long)
*/

BOOST_FIXTURE_TEST_CASE( small_table_test, FitsIO::Test::MinimalFile ) {

  FitsIO::Test::SmallTable input;
  Hdu::create_bintable_extension(this->fptr, "IMGEXT",
      input.num_col, input.radec_col, input.name_col, input.dist_mag_col);
  const auto output_nums = Bintable::read_column<FitsIO::Test::SmallTable::num_t>(this->fptr, input.num_col.info.name);
  check_equal_vectors(output_nums.vector(), input.num_col.vector());
  const auto output_radecs = Bintable::read_column<FitsIO::Test::SmallTable::radec_t>(this->fptr, input.radec_col.info.name);
  check_equal_vectors(output_radecs.vector(), input.radec_col.vector());
  const auto output_names = Bintable::read_column<FitsIO::Test::SmallTable::name_t>(this->fptr, input.name_col.info.name);
  check_equal_vectors(output_names.vector(), input.name_col.vector());
  // const auto output_dists_mags = Bintable::read_column<FitsIO::Test::SmallTable::dist_mag_t>(this->fptr, input.dist_mag_col.info.name);
  // BOOST_CHECK_EQUAL(output_dists_mags.vector().size(), input.dist_mag_col.vector().size());
  // for(std::size_t i=0; i<output_dists_mags.vector().size(); ++i)
  //   check_equal_vectors(output_dists_mags.vector()[i], input.dist_mag_col.vector()[i]);
  //TODO memory bug for vector columns "free(): invalid next size (fast)"

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
