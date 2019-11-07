/**
 * @file tests/src/HeaderWrapper_test.cpp
 * @date 07/23/19
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

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/HeaderWrapper.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (HeaderWrapper_test)

//-----------------------------------------------------------------------------

const double atol = 1e-4;

template<typename T>
void check_close(T value, T expected) {
	BOOST_CHECK_EQUAL(value, expected);
}

template<>
void check_close(float value, float expected) {
	BOOST_CHECK_CLOSE(value, expected, atol);
}

template<>
void check_close(double value, double expected) {
	BOOST_CHECK_CLOSE(value, expected, atol);
}

template<>
void check_close(std::complex<float> value, std::complex<float> expected) {
	BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
	BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template<>
void check_close(std::complex<double> value, std::complex<double> expected) {
	BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
	BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template<typename T>
void check_record(std::string tag) {
	FitsIO::Test::MinimalFile file;
	T input = FitsIO::Test::generate_random_value<T>();
	Header::write_record(file.fptr, FitsIO::Record<T>(tag, input));
	const auto output = Header::parse_record<T>(file.fptr, tag);
	check_close(output.value, input);
}

#define TEST_RECORD_ALIAS(type, name) \
	BOOST_AUTO_TEST_CASE( name##_test ) { check_record<type>(#name); }

#define TEST_RECORD(type) \
	TEST_RECORD_ALIAS(type, type)

#define TEST_RECORD_UNSIGNED(type) \
	TEST_RECORD_ALIAS(unsigned type, u##type)

TEST_RECORD(bool)
TEST_RECORD(char)
TEST_RECORD(short)
TEST_RECORD(int)
TEST_RECORD(long)
TEST_RECORD(float)
TEST_RECORD(double)
TEST_RECORD_ALIAS(std::complex<float>, complex_float)
TEST_RECORD_ALIAS(std::complex<double>, complex_double)
TEST_RECORD_ALIAS(std::string, string)
TEST_RECORD_UNSIGNED(char)
TEST_RECORD_UNSIGNED(short)
TEST_RECORD_UNSIGNED(int)
//TEST_RECORD_UNSIGNED(long)

struct RecordList {
	FitsIO::Record<bool> b { "BOOL" };
	FitsIO::Record<int> i { "INT" };
	FitsIO::Record<double> d { "DOUBLE" };
	FitsIO::Record<std::string> s { "STRING" };
};

BOOST_AUTO_TEST_CASE( struct_io_test ) {
	// FitsIO::Test::MinimalFile file;
	// RecordList input {
	// 		{ "BOOL", true },
	// 		{ "INT", 2 },
	// 		{ "DOUBLE", 3.},
	// 		{ "STRING", "four"}
	// };
	// Header::write_records(file.fptr, input.b, input.i, input.d, input.s);
	// auto t = Header::parse_records<bool, int, double, std::string>(file.fptr,
	// 		{ "BOOL", "INT", "DOUBLE", "STRING" });
	// RecordList output = tuple_to_struct<decltype(t), RecordList>(t);
	// BOOST_CHECK_EQUAL(output.b, input.b);
	// BOOST_CHECK_EQUAL(output.i, input.i);
	// BOOST_CHECK_EQUAL(output.d, input.d);
	// BOOST_CHECK_EQUAL(output.s, input.s);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
