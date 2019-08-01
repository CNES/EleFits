/**
 * @file src/lib/CfitsioFixture.cpp
 * @date 07/31/19
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

#include "EL_CFitsIOWrapper/CfitsioFixture.h"

namespace Cfitsio {
namespace Test {

MinimalFile::MinimalFile() :
		tmp(),
		filename(tmp.path().string()) {
    fptr = File::create_and_open(filename, File::CreatePolicy::OVER_WRITE);
}

SmallRaster::SmallRaster(long width, long height) :
		Raster<float>({width, height}),
		width(width), height(height) {
	for(int x=0; x<shape[0]; ++x)
		for(int y=0; y<shape[1]; ++y)
			operator()({x, y}) = 0.1 * y + x;
}

bool SmallRaster::approx(const Image::Raster<float>& other, float tol) const {
	if(other.shape != this->shape)
		return false;
	for(std::size_t i=0; i<this->size(); ++i) {
		const auto o = other.data[i];
		const auto t = this->data[i];
		const auto rel = (o - t) / t;
		if(rel > 0 && rel > tol)
			return false;
		if(rel < 0 && -rel > tol)
			return false;
	}
	return true;
}

SmallTable::SmallTable() :
		extname("MESSIER"),
		ids { 45, 7, 31 },
		radecs { {56.8500, 24.1167}, {268.4667, -34.7928}, {10.6833, 41.2692} },
		names { "Pleiades", "Ptolemy Cluster", "Andromeda Galaxy" },
		id_col { "Id", 1, "", ids },
		radec_col { "RA/dec", 2, "deg", radecs },
		name_col { "Name", 1, "", names } {
}

}
}

