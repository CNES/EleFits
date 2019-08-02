/**
 * @file tests/src/HduWrapper_test.cpp
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

#include "EL_CFitsIOWrapper//HduWrapper.h"

#include "EL_CFitsIOWrapper//CfitsioFixture.h"

using namespace Cfitsio;

template<typename T>
void check_equal_vectors(const std::vector<T>& a, const std::vector<T>& b) {
	BOOST_CHECK_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (HduWrapper_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( minimal_file_test, Test::MinimalFile ) {

	BOOST_CHECK_EQUAL(HDU::count(this->fptr), 1);
	
}

BOOST_FIXTURE_TEST_CASE( write_read_image_test, Test::MinimalFile ) {

	Test::SmallRaster input;
	HDU::create_image_extension(this->fptr, "IMGEXT", input);
	const auto output = Image::read_raster<float, 2>(fptr);
	BOOST_CHECK(input.approx(output));
	
}

BOOST_FIXTURE_TEST_CASE( write_read_table_test, Test::MinimalFile ) {

	Test::SmallTable input;
	HDU::create_bintable_extension(this->fptr, "IMGEXT",
			input.id_col, input.radec_col, input.name_col, input.dist_mag_col);
	const auto output_ids = Bintable::read_column<Test::SmallTable::id_t>(fptr, input.id_col.name);
	check_equal_vectors(output_ids, input.id_col.data);
	const auto output_radecs = Bintable::read_column<Test::SmallTable::radec_t>(fptr, input.radec_col.name);
	check_equal_vectors(output_radecs, input.radec_col.data);
	const auto output_names = Bintable::read_column<Test::SmallTable::name_t>(fptr, input.name_col.name);
	check_equal_vectors(output_names, input.name_col.data);
	const auto output_dists_mags = Bintable::read_column<Test::SmallTable::dist_mag_t>(fptr, input.dist_mag_col.name);
	for(std::size_t i=0; i<output_dists_mags.size(); ++i)
		check_equal_vectors(output_dists_mags[i], input.dist_mag_col.data[i]);
	
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


