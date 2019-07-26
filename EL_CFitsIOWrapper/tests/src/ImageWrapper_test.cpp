/**
 * @file tests/src/ImageWrapper_test.cpp
 * @date 07/25/19
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
#include <cstdlib>

#include "EL_CFitsIOWrapper//ImageWrapper.h"

using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImageWrapper_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( index_test ) {

    Image::pos_type<4> shape;
    for(auto& length : shape)
        length = std::rand();
    Image::pos_type<4> pos;
    for(auto& coord : pos)
        coord = std::rand();
    auto index = Image::internal::Index<3>::offset(shape, pos);
    BOOST_CHECK_EQUAL(index,
        pos[0] + shape[0] * (
            pos[1] + shape[1] * (
                pos[2] + shape[2] * 
                    (pos[3])
                )
            )
        );

}

BOOST_AUTO_TEST_CASE( raster_2D_test ) {

    std::size_t width(3), height(2);
    std::size_t size(width*height);
    Image::Raster<int> raster({width, height});
    BOOST_CHECK_EQUAL(raster.size(), size);
    BOOST_CHECK_EQUAL(raster.data.size(), size);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


