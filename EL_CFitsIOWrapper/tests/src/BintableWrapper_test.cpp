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

#include "EL_CFitsIOWrapper//BintableWrapper.h"
#include "EL_CFitsIOWrapper//CfitsioFixture.h"

using namespace Cfitsio;

template<typename T>
void check_equal_vectors(const std::vector<T>& a, const std::vector<T>& b) {
	BOOST_CHECK_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BintableWrapper_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_scalar() {
	Test::SmallScalarColumn<T> input;
	Test::MinimalFile file;
	HDU::create_bintable_extension(file.fptr, "BINEXT", input);
	const auto output = Bintable::read_column<T>(file.fptr, input.name);
	check_equal_vectors(input.data, output.data);
}

BOOST_AUTO_TEST_CASE( scalar_test ) {

	check_scalar<char>();

}

void check_string() {
	Test::SmallStringColumn input;
	Test::MinimalFile file;
	HDU::create_bintable_extension(file.fptr, "BINEXT", input);
	const auto output = Bintable::read_column<std::string>(file.fptr, input.name);
	check_equal_vectors(input.data, output.data);
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
		check_equal_vectors(input.data[i], output.data[i]);
}

BOOST_AUTO_TEST_CASE( vector_test ) {

	check_vector<float>();

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


