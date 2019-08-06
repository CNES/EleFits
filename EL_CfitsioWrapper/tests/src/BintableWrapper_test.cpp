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

#include "EL_CfitsioWrapper//BintableWrapper.h"
#include "EL_CfitsioWrapper//CfitsioFixture.h"

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
	Test::RandomScalarColumn<T> input;
	Test::MinimalFile file;
	HDU::create_bintable_extension(file.fptr, "BINEXT", input);
	const auto output = Bintable::read_column<T>(file.fptr, input.name);
	check_equal_vectors(output.data, input.data);
}

#define TEST_SCALAR(type) \
	BOOST_AUTO_TEST_CASE( scalar_##type##_test ) { check_scalar<type>(); }

#define TEST_SCALAR_UNSIGNED(type) \
	BOOST_AUTO_TEST_CASE( scalar_u##type##_test ) { check_scalar<unsigned type>(); }

// TEST_SCALAR(bool) //TODO won't compile
TEST_SCALAR(char)
TEST_SCALAR(short)
TEST_SCALAR(int)
TEST_SCALAR(long)
TEST_SCALAR(LONGLONG)
TEST_SCALAR(float)
TEST_SCALAR(double)
TEST_SCALAR_UNSIGNED(char)
TEST_SCALAR_UNSIGNED(short)
TEST_SCALAR_UNSIGNED(int)
TEST_SCALAR_UNSIGNED(long)


void check_string() {
	Test::SmallStringColumn input;
	Test::MinimalFile file;
	HDU::create_bintable_extension(file.fptr, "BINEXT", input);
	const auto output = Bintable::read_column<std::string>(file.fptr, input.name);
	check_equal_vectors(output.data, input.data);
}

BOOST_AUTO_TEST_CASE( string_test ) {

	check_string();

}

template<typename T>
void check_vector() {
	Test::SmallVectorColumn<T> input;
	Test::MinimalFile file;
	HDU::create_bintable_extension(file.fptr, "BINEXT", input);
	const auto output = Bintable::read_column<std::vector<T>>(file.fptr, input.name);
	const auto size = input.data.size();
	BOOST_CHECK_EQUAL(output.data.size(), size);
	for(std::size_t i=0; i<size; ++i)
		check_equal_vectors(output.data[i], input.data[i]);
}

#define TEST_VECTOR(type) \
	BOOST_AUTO_TEST_CASE( vector_##type##_test ) { check_vector<type>(); }

#define TEST_VECTOR_UNSIGNED(type) \
	BOOST_AUTO_TEST_CASE( vector_u##type##_test ) { check_vector<unsigned type>(); }

// TEST_VECTOR(bool) //TODO won't compile
TEST_VECTOR(char)
TEST_VECTOR(short)
TEST_VECTOR(int)
TEST_VECTOR(long)
TEST_VECTOR(LONGLONG)
TEST_VECTOR(float)
TEST_VECTOR(double)
TEST_VECTOR_UNSIGNED(char)
TEST_VECTOR_UNSIGNED(short)
TEST_VECTOR_UNSIGNED(int)
TEST_VECTOR_UNSIGNED(long)

BOOST_FIXTURE_TEST_CASE( small_table_test, Test::MinimalFile ) {

	Test::SmallTable input;
	HDU::create_bintable_extension(this->fptr, "IMGEXT",
			input.id_col, input.radec_col, input.name_col, input.dist_mag_col);
	const auto output_ids = Bintable::read_column<Test::SmallTable::id_t>(this->fptr, input.id_col.name);
	check_equal_vectors(output_ids.data, input.id_col.data);
	const auto output_radecs = Bintable::read_column<Test::SmallTable::radec_t>(this->fptr, input.radec_col.name);
	check_equal_vectors(output_radecs.data, input.radec_col.data);
	const auto output_names = Bintable::read_column<Test::SmallTable::name_t>(this->fptr, input.name_col.name);
	check_equal_vectors(output_names.data, input.name_col.data);
	const auto output_dists_mags = Bintable::read_column<Test::SmallTable::dist_mag_t>(this->fptr, input.dist_mag_col.name);
	BOOST_CHECK_EQUAL(output_dists_mags.data.size(), input.dist_mag_col.data.size());
	for(std::size_t i=0; i<output_dists_mags.data.size(); ++i)
		check_equal_vectors(output_dists_mags.data[i], input.dist_mag_col.data[i]);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
